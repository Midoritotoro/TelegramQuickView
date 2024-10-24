#include "TelegramParser.h"

#include <iostream>
#include <future>
#include <xutility>
#include <ctime>

#include <QString>
#include <QDebug>


TelegramParser::TelegramParser():
    AbstractTelegramParser()
    , _sqlManager(std::make_unique<PostSqlManager>())
{
    setlocale(LC_ALL, "");

    foreach(const auto& element, _userDataManager->getUsernamesOfTargetChannels())
        _targetChannelsList.append(element.toString());

    foreach(const auto& channel, _targetChannelsList) {
        sendQuery(td::td_api::make_object<td::td_api::searchPublicChat>(channel.toStdString()),
            [this](Object object) {
                if (object == nullptr)
                    return;

                if (object->get_id() == td::td_api::error::ID) {
                    auto error = td::move_tl_object_as<td::td_api::error>(object);
                    std::cout << "Error: " << to_string(error) << std::flush;

                    handleError(error);
                }

                const auto chat = td::move_tl_object_as<td::td_api::chat>(object);
                if (chat)
                    _userDataManager->setTargetChannelsChatIds(QList<qint64>({ chat->id_ }));
            }
        );
    }

    connect(this, &AbstractTelegramParser::userAuthorized, [this]() {
      /*  if (_Future.valid())
            _Future.get();
        _Future = std::async(&TelegramParser::startChatsChecking, this);*/
    });

    foreach(const auto& id, _userDataManager->getIdsOfTargetChannels())
        _chatIdsVector.push_back(id.toLongLong());

    if (isAuthorized())
        _Future = std::async(&TelegramParser::startChatsChecking, this);
}

void TelegramParser::startChatsChecking() {
    for (;;)
        processResponse(_clientManager->receive(10));
}


auto TelegramParser::createFileDownloadQueryHandler() {
    return [this](Object object) {
        if (object == nullptr)
            return;
        
        auto file = move_tl_object_as<td::td_api::file>(object);
        auto it = _downloadingMessages.find(file->id_);

        if (it != _downloadingMessages.end()) {
            qDebug() << "file->local_->is_downloading_completed_: " << file->local_->is_downloading_completed_ << " File size: " << file->size_ / (1024*1024) << " Mb";
            qDebug() << "Path to file: " << file->local_->path_.c_str();
            if (file->local_->is_downloading_completed_)
                it->second.attachment = file->local_->path_.c_str();

            _sqlManager->writeMessageInfo(it->second);
            _downloadingMessages.erase(it);
        }

       // checkFileDownloadError(std::move(object));
    };
}

void TelegramParser::checkFileDownloadError(Object object) {
    if (object->get_id() == td::td_api::error::ID) {
        auto error = td::move_tl_object_as<td::td_api::error>(object);
        std::cout << "Error: " << to_string(error) << std::flush;

        handleError(error);
        on_NewMessageUpdate(std::move(object));
    }
}

void TelegramParser::processResponse(td::ClientManager::Response response) {
    if (!response.object)
        return;

    if (response.request_id == 0) 
        return on_NewMessageUpdate(std::move(response.object));
    
    auto iterator = _handlers.find(response.request_id);

    if (iterator != _handlers.end()) {
        iterator->second(std::move(response.object));
        _handlers.erase(iterator);
    }
}

std::string TelegramParser::getUserName(std::int64_t user_id) const {
    auto it = users_.find(user_id);

    return it == users_.end() ? 
        "unknown user" 
        : it->second->first_name_ + 
        " " + it->second->last_name_;
}

std::string TelegramParser::getChatTitle(std::int64_t chat_id) const {
    auto it = chat_title_.find(chat_id);

    return it == chat_title_.end() 
        ? "unknown chat" 
        : it->second;
}

void TelegramParser::on_NewMessageUpdate(td::td_api::object_ptr<td::td_api::Object> update) {
    td::td_api::downcast_call(
        *update, overloaded(
            [this](td::td_api::updateNewChat& update_new_chat) {  
                chat_title_[update_new_chat.chat_->id_] = update_new_chat.chat_->title_;
            },
            [this](td::td_api::updateChatTitle& update_chat_title) {
                chat_title_[update_chat_title.chat_id_] = update_chat_title.title_;
            },
            [this](td::td_api::updateUser& update_user) {
                const auto user_id = update_user.user_->id_;
                users_[user_id] = std::move(update_user.user_);
            },
            [this](td::td_api::updateNewMessage& update_new_message) {
                auto chatId = update_new_message.message_->chat_id_;
                bool isTarget = false;

                for (int index = 0; index < _chatIdsVector.size(); ++index) 
                    if (_chatIdsVector[index] == chatId) {
                        isTarget = true;
                        break;
                    }

                if (isTarget == false)
                    return;

                std::string senderName;

                td::td_api::downcast_call(*update_new_message.message_->sender_id_,
                    overloaded(
                        [this, &senderName](td::td_api::messageSenderUser& user) {
                            senderName = getUserName(user.user_id_);
                        },
                        [this, &senderName](td::td_api::messageSenderChat& chat) {
                            senderName = getChatTitle(chat.chat_id_);
                        }
                    )
                );

                qDebug() << "Получено сообщение от: " << senderName;

                std::string text;
                int32_t mediaId = 0;

                switch (update_new_message.message_->content_->get_id()) {
                    case td::td_api::messageText::ID:
                        text = static_cast<td::td_api::messageText&>(*update_new_message.message_->content_).text_->text_;
                        break;

                    case td::td_api::messagePhoto::ID:
                        mediaId = static_cast<td::td_api::messagePhoto&>(*update_new_message.message_->content_).photo_->sizes_.back()->photo_->id_;
                        break;

                    case td::td_api::messageVideo::ID:
                        mediaId = static_cast<td::td_api::messageVideo&>(*update_new_message.message_->content_).video_->video_->id_;
                        break;

                    case td::td_api::messageDocument::ID:
                        mediaId = static_cast<td::td_api::messageDocument&>(*update_new_message.message_->content_).document_->document_->id_;
                        break;
                }

                TelegramMessage message;

                message.date = convertTdMessageTimestamp(update_new_message.message_->date_).c_str();
                message.sender = senderName.c_str();
                message.text = text.c_str();
                message.mediaAlbumId = update_new_message.message_->media_album_id_;

                qDebug() << "mediaId: " << mediaId;
                qDebug() << "mediaAlbumId: " << message.mediaAlbumId;
       
                if (mediaId == 0) {
                    qDebug() << "Запись данных о новом сообщении без медиа...";
                    _sqlManager->writeMessageInfo(message);
                    return;
                }
                
                //if (message.mediaAlbumId == 0) { // Сообщение с одним медиа
                //    qDebug() << "Запись данных о новом сообщении с одним медиа...";
                //    _downloadingMessages[mediaId] = message;
                //    sendQuery(
                //        td::td_api::make_object<td::td_api::downloadFile>(mediaId, 32, 0, 0, false),
                //        createFileDownloadQueryHandler()
                //    );

                //    return;
                //}

                _downloadingMessages[mediaId] = message;
                
                sendQuery(
                    td::td_api::make_object<td::td_api::downloadFile>(mediaId, 32, 0, 0, true),
                    createFileDownloadQueryHandler()
                );      
            },

            [](auto& update) {

            }
        )
    );

    processUpdate(std::move(update));
}

std::string TelegramParser::convertTdMessageTimestamp(int64_t time) {
    char buffer[20];

    time_t _time = static_cast<time_t>(time);
    tm* localTime = localtime(&_time);

    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
    std::string formattedDate = buffer;

    return formattedDate;
}