#include "TelegramAuthorizer.h"

#include <cstdint>
#include <iostream>
#include <vector>
#include <QString>
#include <QDebug>


TelegramAuthorizer::TelegramAuthorizer(QObject* parent):
    QObject(parent)
{
    td::ClientManager::execute(td::td_api::make_object<td::td_api::setLogVerbosityLevel>(1));

    client_manager_ = std::make_unique<td::ClientManager>();
    client_id_ = client_manager_->create_client_id();

    send_query(td::td_api::make_object<td::td_api::getOption>("version"), {});
}


void TelegramAuthorizer::loop() {
    while (true) {
        if (are_authorized_)
            break;
        else
            process_response(client_manager_->receive(10));
    }
}

bool TelegramAuthorizer::auth() {
    int count = 0;

    while (true) {
        if (are_authorized_)
            return true;
        else 
            process_response(client_manager_->receive(10));
        if (count > 5)
            break;
        ++count;
        _sleep(100);
    }
    return false;
}

bool TelegramAuthorizer::isCredentialsAccepted() {
    return _isCredentialsAccepted;
}

bool TelegramAuthorizer::isAuthorized() {
    return _isAuthCodeAccepted;
}

bool TelegramAuthorizer::setTelegramCredentials(const TelegramCredentials& credentials) {
    _telegramCredentials = credentials;

    while (true) {
        qDebug() << "credentialsSet in process...";
        process_response(client_manager_->receive(10));

        if (authorization_state_.get() == nullptr)
            continue;

        qDebug() << "auth state:" << (authorization_state_.get()->get_id() == td::td_api::authorizationStateWaitCode::ID);

        if (authorization_state_.get()->get_id() == td::td_api::authorizationStateWaitCode::ID)
            return _isCredentialsAccepted;
    }

    return _isCredentialsAccepted;
}

bool TelegramAuthorizer::setAuthorizationCode(std::string code) {
    _authorizationCode = code;

    while (true) {
        process_response(client_manager_->receive(10));

        if (authorization_state_.get() == nullptr)
            continue;

        if (authorization_state_.get()->get_id() == td::td_api::authorizationStateReady::ID)
            break;
    }

    return _isAuthCodeAccepted;
}

void TelegramAuthorizer::restart() {
    client_manager_.reset();
}

void TelegramAuthorizer::send_query(td::td_api::object_ptr<td::td_api::Function> f, std::function<void(Object)> handler) {
    auto query_id = next_query_id();
    if (handler) {
        handlers_.emplace(query_id, std::move(handler));
    }
    client_manager_->send(client_id_, query_id, std::move(f));
}

void TelegramAuthorizer::process_response(td::ClientManager::Response response) {
    if (!response.object) {
        return;
    }
    if (response.request_id == 0) {
        return process_update(std::move(response.object));
    }
    auto it = handlers_.find(response.request_id);
    if (it != handlers_.end()) {
        it->second(std::move(response.object));
        handlers_.erase(it);
    }
}

std::string TelegramAuthorizer::get_user_name(std::int64_t user_id) const {
    auto it = users_.find(user_id);
    if (it == users_.end()) {
        return "unknown user";
    }
    return it->second->first_name_ + " " + it->second->last_name_;
}

std::string TelegramAuthorizer::get_chat_title(std::int64_t chat_id) const {
    auto it = chat_title_.find(chat_id);
    if (it == chat_title_.end()) {
        return "unknown chat";
    }
    return it->second;
}

void TelegramAuthorizer::process_update(td::td_api::object_ptr<td::td_api::Object> update) {
    td::td_api::downcast_call(
        *update, overloaded(
            [this](td::td_api::updateAuthorizationState& update_authorization_state) {
                qDebug() << "td::td_api::updateAuthorizationState&";
                authorization_state_ = std::move(update_authorization_state.authorization_state_);
                on_authorization_state_update();
            },
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
                            sender_name = get_user_name(user.user_id_);
                        },
                        [this, &sender_name](td::td_api::messageSenderChat& chat) {
                            sender_name = get_chat_title(chat.chat_id_);
                        }));
                std::string text;
                if (update_new_message.message_->content_->get_id() == td::td_api::messageText::ID) {
                    text = static_cast<td::td_api::messageText&>(*update_new_message.message_->content_).text_->text_;
                }
                std::cout << "Receive message: [chat_id:" << chat_id << "] [from:" << sender_name << "] ["
                    << text << "]" << std::endl;
            },
            [](auto& update) {}));
}

auto TelegramAuthorizer::create_authentication_query_handler() {
    return [this, id = authentication_query_id_](Object object) {
        if (id == authentication_query_id_) {
            check_authentication_error(std::move(object));
        }
        };
}

void TelegramAuthorizer::on_authorization_state_update() {
    authentication_query_id_++;
    td::td_api::downcast_call(*authorization_state_,
        overloaded(
            [this](td::td_api::authorizationStateReady&) {
                qDebug() << "td::td_api::authorizationStateReady&";
                _isAuthCodeAccepted = true;
                are_authorized_ = true;
                _isCredentialsAccepted = true;
            },
            [this](td::td_api::authorizationStateLoggingOut&) {
                qDebug() << "td::td_api::authorizationStateLoggingOut&";
                _isAuthCodeAccepted = false;
                are_authorized_ = false;
                _isCredentialsAccepted = false;
            },
            [this](td::td_api::authorizationStateClosing&) {
                qDebug() << "td::td_api::authorizationStateClosing&";
            },
            [this](td::td_api::authorizationStateClosed&) {
                qDebug() << "td::td_api::authorizationStateClosed&";
                _isAuthCodeAccepted = false;
                are_authorized_ = false;
                _isCredentialsAccepted = false;
            },
            [this](td::td_api::authorizationStateWaitEmailAddress&) {
                qDebug() << "td::td_api::authorizationStateWaitEmailAddress&";
            },
            [this](td::td_api::authorizationStateWaitEmailCode&) {
                qDebug() << "td::td_api::authorizationStateWaitEmailCode&";
            },
            [this](td::td_api::authorizationStateWaitRegistration&) {
                qDebug() << "td::td_api::authorizationStateWaitRegistration&";
            },
            [this](td::td_api::authorizationStateWaitPassword&) {
                qDebug() << "td::td_api::authorizationStateWaitPassword&";
                _isCredentialsAccepted = true;
            },
            [this](td::td_api::authorizationStateWaitPhoneNumber&) {
                qDebug() << "td::td_api::authorizationStateWaitPhoneNumber&";
                _isCredentialsAccepted = false;

                send_query(
                    td::td_api::make_object<td::td_api::setAuthenticationPhoneNumber>(_telegramCredentials.phoneNumber, nullptr),
                    create_authentication_query_handler()
                );
            },
            [this](td::td_api::authorizationStateWaitCode&) {
                qDebug() << "td::td_api::authorizationStateWaitCode&";
                _isAuthCodeAccepted = false;
                _isCredentialsAccepted = true;
               // emit authorizationCodeNeeded();

                if (_authorizationCode.empty() == true) {
                    qDebug() << "_authorizationCode is Empty";
                    return;
                }
                qDebug() << "td::td_api::authorizationStateWaitCode& sended";
                send_query(
                    td::td_api::make_object<td::td_api::checkAuthenticationCode>(_authorizationCode),
                    create_authentication_query_handler()
                );
            },
            [this](td::td_api::authorizationStateWaitOtherDeviceConfirmation& state) {
                _isCredentialsAccepted = true;
                std::cout << "Confirm this login link on another device: " << state.link_ << std::endl;
            },
            [this](td::td_api::authorizationStateWaitTdlibParameters&) {
                qDebug() << "td::td_api::authorizationStateWaitTdlibParameters&";
                _isCredentialsAccepted = false;

                if (_telegramCredentials.isEmpty())
                    return;
                qDebug() << "credentialsSet";
                auto request = td::td_api::make_object<td::td_api::setTdlibParameters>();

                request->api_id_ = _telegramCredentials.apiId;
                request->api_hash_ = _telegramCredentials.apiHash;

                request->database_directory_ = "tdlib";
                request->use_message_database_ = true;
                request->use_secret_chats_ = true;

                request->system_language_code_ = "en";
                request->device_model_ = "Desktop";
                request->application_version_ = "1.0";

                send_query(std::move(request), create_authentication_query_handler());
            }));
}

void TelegramAuthorizer::check_authentication_error(Object object) {
    if (object->get_id() == td::td_api::error::ID) {
        auto error = td::move_tl_object_as<td::td_api::error>(object);
        std::cout << "Error: " << to_string(error) << std::flush;
        on_authorization_state_update();
    }
}

std::uint64_t TelegramAuthorizer::next_query_id() {
    return ++current_query_id_;
}