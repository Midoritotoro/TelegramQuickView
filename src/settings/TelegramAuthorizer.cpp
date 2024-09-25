#include "TelegramAuthorizer.h"

#include <cstdint>
#include <iostream>
#include <vector>
#include <QString>
#include <QDebug>


TelegramAuthorizer::TelegramAuthorizer()
{
    td::ClientManager::execute(td::td_api::make_object<td::td_api::setLogVerbosityLevel>(1));

    client_manager_ = std::make_unique<td::ClientManager>();
    client_id_ = client_manager_->create_client_id();

    send_query(td::td_api::make_object<td::td_api::getOption>("version"), {});
}

bool TelegramAuthorizer::isCredentialsAccepted() {
    return _isCredentialsAccepted;
}

bool TelegramAuthorizer::isAuthorized() {
    return _isAuthCodeAccepted;
}

void TelegramAuthorizer::setTelegramCredentials(const TelegramCredentials& credentials) {
    _telegramCredentials = credentials;

    while (true) {
        process_response(client_manager_->receive(10));

        if (authorization_state_.get() == nullptr)
            continue;

        if (authorization_state_.get()->get_id() == td::td_api::authorizationStateWaitCode::ID
            || authorization_state_.get()->get_id() == td::td_api::authorizationStateReady::ID) {
            return;
        }
    }
}

void TelegramAuthorizer::setAuthorizationCode(std::string code) {
    _authorizationCode = code;

    while (true) {
        process_response(client_manager_->receive(10));

        if (authorization_state_.get() == nullptr)
            continue;

        if (authorization_state_.get()->get_id() == td::td_api::authorizationStateReady::ID)
            return;
    }
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

void TelegramAuthorizer::process_update(td::td_api::object_ptr<td::td_api::Object> update) {
    td::td_api::downcast_call(
        *update, overloaded(
            [this](td::td_api::updateAuthorizationState& update_authorization_state) {
                authorization_state_ = std::move(update_authorization_state.authorization_state_);
                on_authorization_state_update();
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
                _isAuthCodeAccepted = true;
                are_authorized_ = true;
                _isCredentialsAccepted = true;
            },
            [this](td::td_api::authorizationStateLoggingOut&) {
                _isAuthCodeAccepted = false;
                are_authorized_ = false;
                _isCredentialsAccepted = false;
            },
            [this](td::td_api::authorizationStateClosed&) {
                _isAuthCodeAccepted = false;
                are_authorized_ = false;
                _isCredentialsAccepted = false;
            },
            [this](td::td_api::authorizationStateWaitPhoneNumber&) {
                _isCredentialsAccepted = false;

                send_query(
                    td::td_api::make_object<td::td_api::setAuthenticationPhoneNumber>(_telegramCredentials.phoneNumber, nullptr),
                    create_authentication_query_handler()
                );
            },
            [this](td::td_api::authorizationStateWaitCode&) {
                _isAuthCodeAccepted = false;
                _isCredentialsAccepted = true;

                if (_authorizationCode.empty() == true) 
                    return;

                send_query(
                    td::td_api::make_object<td::td_api::checkAuthenticationCode>(_authorizationCode),
                    create_authentication_query_handler()
                );
            },
            [this](td::td_api::authorizationStateWaitTdlibParameters&) {
                _isCredentialsAccepted = false;

                if (_telegramCredentials.isEmpty())
                    return;

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
            },
            [](auto& update) {}));
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