#include "TelegramParser.h"

#include <iostream>


TelegramParser::TelegramParser() :
	TelegramAuthorizer()
{
    _sqlManager = std::make_unique<PostSqlManager>();

    for (;;) {
        processResponse(_clientManager->receive(10));
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

    if (it == users_.end()) 
        return "unknown user";

    return it->second->first_name_ + " " + it->second->last_name_;
}

std::string TelegramParser::getChatTitle(std::int64_t chat_id) const {
    auto it = chat_title_.find(chat_id);

    if (it == chat_title_.end())
        return "unknown chat";
    
    return it->second;
}

void TelegramParser::on_NewMessageUpdate(td::td_api::object_ptr<td::td_api::Object> update) {
    processUpdate(std::move(update));

    td::td_api::downcast_call(
        *update, overloaded(
            [this](td::td_api::updateNewChat& update_new_chat) {
                chat_title_[update_new_chat.chat_->id_] = update_new_chat.chat_->title_;
            },
            [this](td::td_api::updateChatTitle& update_chat_title) {
                chat_title_[update_chat_title.chat_id_] = update_chat_title.title_;
            },
            [this](td::td_api::updateUser& update_user) {
                auto user_id = update_user.user_->id_;
                users_[user_id] = std::move(update_user.user_);
            },
            [this](td::td_api::updateNewMessage& update_new_message) {
                auto chat_id = update_new_message.message_->chat_id_;
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

                if (update_new_message.message_->content_->get_id() == td::td_api::messageText::ID) {
                    text = static_cast<td::td_api::messageText&>(*update_new_message.message_->content_).text_->text_;
                }
                else if (update_new_message.message_->content_->get_id() == td::td_api::messagePhoto::ID) {
                    mediaId = static_cast<td::td_api::messagePhoto&>(*update_new_message.message_->content_).photo_->sizes_.back()->photo_->id_;
                }
                else if (update_new_message.message_->content_->get_id() == td::td_api::messageVideo::ID) {
                    mediaId = static_cast<td::td_api::messageVideo&>(*update_new_message.message_->content_).video_->video_->id_;
                }
                else if (update_new_message.message_->content_->get_id() == td::td_api::messageAudio::ID) {
                    mediaId = static_cast<td::td_api::messageAudio&>(*update_new_message.message_->content_).audio_->audio_->id_;
                }

            /*    if (mediaId != 0)
                    td::td_api::downloadFile::downloadFile(mediaId, 32, 0, 0, false);*/

                std::cout << "Receive message: [chat_id:" << chat_id << "] [from:" << sender_name << "] ["
                    << text << "]" << std::endl;
            },
            [](auto& update) {}));
}

