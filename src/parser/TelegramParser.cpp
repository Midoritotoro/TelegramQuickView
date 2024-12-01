#include "TelegramParser.h"

#include <iostream>
#include <future>
#include <xutility>
#include <ctime>

#include <QString>
#include <QDebug>

#include <QMutex>

TelegramParser::TelegramParser():
    AbstractTelegramParser()
    , _downloadSensitiveContent(_userDataManager->getDownloadSensitiveContentAbility())
    , _countOfLatestDownloadingMessages(_userDataManager->getCountOfLatestDownloadingMessages())
{
    foreach(const auto & element, _userDataManager->getUsernamesOfTargetChannels())
        _targetChannelsList.append(element.toString());

    foreach(const auto & channel, _targetChannelsList) {
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
        
    });

    foreach(const auto& id, _userDataManager->getIdsOfTargetChannels())
        _chatIdsVector.push_back(id.toLongLong());

    moveToThread(_thread);
    if (!_thread->isRunning())
        _thread->start();
}

TelegramParser::~TelegramParser() {
    _thread->quit();
}

auto TelegramParser::createFileDownloadQueryHandler() {
    return [this](Object object) {
        if (object == nullptr || object->get_id() != td::td_api::file::ID)
            return checkFileDownloadError(std::move(object));

        auto file = td::move_tl_object_as<td::td_api::file>(object);
        if (file == nullptr || file->size_ == 0)
            return;

        auto it = _downloadingMessages.find(file->id_);

        if (it != _downloadingMessages.end()) {
            qDebug() << "file->local_->is_downloading_completed_: " << file->local_->is_downloading_completed_ << " File size: " << file->size_ / (1024 * 1024) << " Mb";
            qDebug() << "Path to file: " << file->local_->path_.c_str();
            if (file->local_->is_downloading_completed_)
                it->second.attachments.append(file->local_->path_.c_str());

            if (_downloadedMessages.size() != 0)
                _downloadedMessages[_downloadedMessages.size() - 1].
                    attachments.append(file->local_->path_.c_str());
            else
                _downloadedMessages[0].
                attachments.append(file->local_->path_.c_str());
   
            _downloadingMessages.erase(it);

           // QMutexLocker locker(&_mutex);
            _downloadedFiles++;
           // _waitCondition.wakeOne();
        }
    };
}

auto TelegramParser::createHistoryRequestHandler() {
    return [this](Object object) {
        if (object == nullptr || object->get_id() == td::td_api::error::ID) {
            qDebug() << "object == nullptr || object->get_id() != td::td_api::messages::ID";
            return checkFileDownloadError(std::move(object));
        }

        auto messages = td::move_tl_object_as<td::td_api::messages>(object);
        qDebug() << "messages->total_count_: " << messages->total_count_;
        if (messages == nullptr || messages->total_count_ == 0)
            return;

        _downloadedMessages.clear();
        _totalFilesToDownload = 0;

        for (auto index = 0; index < messages->total_count_;) {
            if (index > _countOfLatestDownloadingMessages)
                break;

            auto chatMessage = std::move(messages->messages_[index]);
            std::string sender = "";

            std::string text = "";
            int64_t mediaId = 0;

            td::td_api::downcast_call(*chatMessage->sender_id_,
                Telegram::overloaded(
                    [this, &sender](td::td_api::messageSenderUser& user) {
                        sender = getUserName(user.user_id_);
                    },
                    [this, &sender](td::td_api::messageSenderChat& chat) {
                        sender = getChatTitle(chat.chat_id_);
                    }
                )
            );

            if (mediaId != 0)
                _totalFilesToDownload++;

            if (_downloadedMessages.back().mediaAlbumId != chatMessage->media_album_id_
                || _downloadedMessages.back().mediaAlbumId == 0) // Новое сообщение
                ++index;

            _downloadedMessages.push_back(Telegram::Message());

            _downloadedMessages[_downloadedMessages.size() - 1].sender = sender.c_str();
            _downloadedMessages[_downloadedMessages.size() - 1].date = convertTdMessageTimestamp(chatMessage->date_).c_str();
            _downloadedMessages[_downloadedMessages.size() - 1].mediaAlbumId = chatMessage->media_album_id_;

            parseMessageContent(std::exchange(chatMessage, {}), text, mediaId);

            sendQuery(
                td::td_api::make_object<td::td_api::downloadFile>(mediaId, 32, 0, 0, false),
                createFileDownloadQueryHandler()
            );

            _downloadedMessages[_downloadedMessages.size() - 1].text = QString::fromStdString(text);
        }

     /*  if (_totalFilesToDownload > 0) {
            QMutexLocker locker(&_mutex);
            _waitCondition.wait(&_mutex, QDeadlineTimer::Forever);
       }*/

        emit messagesLoaded();
    };
}

Telegram::Message TelegramParser::loadMessage() {
    if (!_downloadedMessages.empty()) {
        auto message = _downloadedMessages.back();
        _downloadedMessages.pop_back();

        return message;
    }

    qDebug() << "loading messages history from channel with id: " << _chatIdsVector[0] << " - " << _targetChannelsList[0];

    sendQuery(
        td::td_api::make_object<td::td_api::getChatHistory>(_chatIdsVector[0], 0, 
        -_countOfLatestDownloadingMessages, _countOfLatestDownloadingMessages, false),
        createHistoryRequestHandler()
    );


    bool needBreak = false;
    connect(this, &TelegramParser::messagesLoaded, [=, &needBreak]() {
        qDebug() << "messagesLoaded";
        needBreak = true;
    });

    while (!needBreak)
        processResponse(_clientManager->receive(10));
    
    if (_downloadedMessages.empty())
        return Telegram::Message();
    
    auto message = _downloadedMessages.back();
    _downloadedMessages.pop_back();

    return message;
}

void TelegramParser::parseMessageContent(td::td_api::object_ptr<td::td_api::message>&& message, std::string& text, int64_t& mediaId) {
    switch (message->content_->get_id()) {
        case td::td_api::messageText::ID:
            text = static_cast<td::td_api::messageText&>(*message->content_).text_->text_;
            break;

        case td::td_api::messagePhoto::ID:
            mediaId = static_cast<td::td_api::messagePhoto&>(*message->content_).photo_->sizes_.back()->photo_->id_;
            break;

        case td::td_api::messageVideo::ID:
            mediaId = static_cast<td::td_api::messageVideo&>(*message->content_).video_->video_->id_;
            break;

        case td::td_api::messageDocument::ID:
            mediaId = static_cast<td::td_api::messageDocument&>(*message->content_).document_->document_->id_;
            break;
    }
}

void TelegramParser::startChatsChecking() {
    while (true)
        processResponse(_clientManager->receive(10));
}

void TelegramParser::checkFileDownloadError(Object object) {
    if (object == nullptr || object->get_id() != td::td_api::error::ID)
        return;

    auto error = td::move_tl_object_as<td::td_api::error>(object);
    std::cout << "Error: " << to_string(error) << std::flush;

    handleError(error);
    on_NewMessageUpdate(std::move(object));
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
        *update, Telegram::overloaded(
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
                if (update_new_message.message_->has_sensitive_content_ && _downloadSensitiveContent == false) {
                    qDebug() << "Message has a 18+ content, so it`s has been skipped";
                    return;
                }

                auto chatId = update_new_message.message_->chat_id_;
                bool isTarget = false;
                QString sender = "";

                for (int index = 0; index < _chatIdsVector.size(); ++index) 
                    if (_chatIdsVector[index] == chatId) {
                        isTarget = true;
                        break;
                    }

                if (isTarget == false)
                    return;

                td::td_api::downcast_call(*update_new_message.message_->sender_id_,
                    Telegram::overloaded(
                        [this, &sender](td::td_api::messageSenderUser& user) {
                            sender = getUserName(user.user_id_).c_str();
                        },
                        [this, &sender](td::td_api::messageSenderChat& chat) {
                            sender = getChatTitle(chat.chat_id_).c_str();
                        }
                    )
                );


                qDebug() << "Получено сообщение от: " << sender;

                std::string text;
                int64_t mediaId = 0;

                parseMessageContent(std::move(update_new_message.message_), text, mediaId);

                Telegram::Message message;

                message.date = convertTdMessageTimestamp(update_new_message.message_->date_).c_str();
                message.sender = sender;
                message.text = text.c_str();
                message.mediaAlbumId = update_new_message.message_->media_album_id_;

                qDebug() << "mediaId: " << mediaId;
                qDebug() << "mediaAlbumId: " << message.mediaAlbumId;
                qDebug() << "message text: " << message.text;

                if (mediaId == 0) {
                    _downloadedMessages.push_back(message);
                    emit messagesLoaded();
                    return;
                }

                const auto previousDownloadedMessage = _downloadingMessages.end()->second;
                if (!message.isNull())
                    if (message.mediaAlbumId == 0 || (message.mediaAlbumId != 0 &&
                        previousDownloadedMessage.mediaAlbumId != message.mediaAlbumId)) {
                        _downloadedMessages.push_back(previousDownloadedMessage);
                        emit messagesLoaded();
                    }

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