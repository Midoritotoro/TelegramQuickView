#include "TelegramParser.h"

#include <iostream>
#include <future>

#include <QString>
#include <QDebug>

TelegramParser::TelegramParser() :
	TelegramAuthorizer()
    , _sqlManager(std::make_unique<PostSqlManager>())
{
    foreach(const auto& element, _userDataManager->getTargetChannels())
        _targetChannelsList.append(element.toString());
    
    connect(this, &TelegramAuthorizer::userAuthorized, [this]() {
        _Future = std::async(std::launch::async,
            &TelegramParser::startChatsChecking, this);
    });
}

void TelegramParser::startChatsChecking() {
    for (;;) {
        processResponse(_clientManager->receive(10));
    }
}


auto TelegramParser::createFileDownloadQueryHandler() {
    return [this](Object object) {
        if (object)
            checkFileDownloadError(std::move(object));
        };
}

void TelegramParser::checkFileDownloadError(Object object) {
    if (object->get_id() == td::td_api::error::ID) {
        auto error = td::move_tl_object_as<td::td_api::error>(object);
        std::cout << "Error: " << to_string(error) << std::flush;
        on_NewMessageUpdate(std::move(object));
    }
}

void TelegramParser::processResponse(td::ClientManager::Response response) {
    if (!response.object)
        return;

    if (response.request_id == 0) {
        return on_NewMessageUpdate(std::move(response.object));
    }
    
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
    setlocale(LC_ALL, "Rus");

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
                auto chat_id = update_new_message.message_->chat_id_;

                bool isTarget = false;

                foreach(const auto& channel, _targetChannelsList) {
                    sendQuery(
                        td::td_api::make_object<td::td_api::searchPublicChat>(channel.toStdString()),
                        [this, chat_id, &isTarget](Object object) {
                            if (object->get_id() == td::td_api::error::ID)
                                return;

                            const auto chat = td::move_tl_object_as<td::td_api::chat>(object);
                            
                            std::cout << chat->id_ << chat_id;
                            if (chat->id_ == chat_id)
                                isTarget = true;
                        }
                    );

                    if (isTarget)
                        break;
                }

                if (isTarget == false) {
                    std::cout << "Канал не находится в списке целей для парсера" << "Returned";
                    return;
                }

                std::string sender_name;

                td::td_api::downcast_call(*update_new_message.message_->sender_id_,
                    overloaded(
                        [this, &sender_name](td::td_api::messageSenderUser& user) {
                            sender_name = getUserName(user.user_id_);
                        },
                        [this, &sender_name](td::td_api::messageSenderChat& chat) {
                            sender_name = getChatTitle(chat.chat_id_);
                        }));

                std::string text;
                td::td_api::int32 mediaId = 0;

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

                if (mediaId > 0)
                    sendQuery(
                        td::td_api::make_object<td::td_api::downloadFile>(mediaId, 32, 0, 0, true),
                        createFileDownloadQueryHandler()
                    );
            },
            [](auto& update) {}));

    processUpdate(std::move(update));
}
