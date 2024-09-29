#include "TelegramAuthorizer.h"

#include <cstdint>
#include <iostream>
#include <vector>

#include <QString>
#include <QDebug>
#include <chrono>


TelegramAuthorizer::TelegramAuthorizer() :
      _clientId(0)
    , _isCredentialsAccepted(false)
    , _isAuthCodeAccepted(false)
    , _currentQueryId(0)
    , _authenticationQueryId(0)
    
{
    td::ClientManager::execute(td::td_api::make_object<td::td_api::setLogVerbosityLevel>(3));

    _clientManager = std::make_unique<td::ClientManager>();
    _clientId = _clientManager->create_client_id();

    sendQuery(td::td_api::make_object<td::td_api::getOption>("version"), {});
}

void TelegramAuthorizer::setTelegramCredentials(const TelegramCredentials& credentials) {
    _telegramCredentials = credentials;

    while (true) {
        processResponse(_clientManager->receive(10));
        qDebug() << "setCred";

        if (_authorizationState == nullptr)
            continue;
   
        if (_authorizationState->get_id() == td::td_api::authorizationStateWaitCode::ID
            || _authorizationState->get_id() == td::td_api::authorizationStateReady::ID
        ) {
            qDebug() << "cred returned";
            return;
        }
    }
}

void TelegramAuthorizer::setAuthorizationCode(std::string code) {
    _authorizationCode = code;

    while (true) {
        qDebug() << "setCode";
        processResponse(_clientManager->receive(10));

        if (_authorizationState == nullptr)
            continue;

        if (_authorizationState->get_id() == td::td_api::authorizationStateReady::ID) {
            qDebug() << "setCode Returned";
            return;
        }
    }
}

auto TelegramAuthorizer::createAuthenticationQueryHandler() {
    return [this, id = _authenticationQueryId](Object object) {
        if (id == _authenticationQueryId)
            checkAuthenticationError(std::move(object));
        };
}

void TelegramAuthorizer::sendTelegramAuthCode() {
    if (_authorizationState->get_id() == td::td_api::authorizationStateWaitCode::ID) {
        sendQuery(
            td::td_api::make_object<td::td_api::resendAuthenticationCode>(),
            createAuthenticationQueryHandler()
        );
    }
}

void TelegramAuthorizer::sendQuery(td::td_api::object_ptr<td::td_api::Function> f, std::function<void(Object)> handler) {
    const auto queryId = nextQueryId();

    if (handler)
        _handlers.emplace(queryId, std::move(handler));

    _clientManager->send(_clientId, queryId, std::move(f));
}

void TelegramAuthorizer::processResponse(td::ClientManager::Response response) {
    if (!response.object)
        return;


    if (response.request_id == 0)
        return processUpdate(std::move(response.object));

    auto iterator = _handlers.find(response.request_id);

    if (iterator != _handlers.end()) {
        iterator->second(std::move(response.object));
        _handlers.erase(iterator);
    }
}

void TelegramAuthorizer::processUpdate(td::td_api::object_ptr<td::td_api::Object> update) {
    _t = false;
    td::td_api::downcast_call(
        *update, overloaded(
            [this](td::td_api::updateAuthorizationState& update_authorization_state) {
                _authorizationState = std::move(update_authorization_state.authorization_state_);
                on_authorizationStateUpdate();
                _t = true;
            },
            [this](td::td_api::updateConnectionState&) {
                if (!_t)
                    on_authorizationStateUpdate();
            },
            [this](auto& update) {

            }
        )
    );
}

void TelegramAuthorizer::on_authorizationStateUpdate() {
    _authenticationQueryId++;

    td::td_api::downcast_call(*_authorizationState,
        overloaded(
            [this](td::td_api::authorizationStateReady&) {
                qDebug() << "td::td_api::authorizationStateReady&";
                _isAuthCodeAccepted = true;
                _telegramCredentials.isEmpty() ? _isCredentialsAccepted = false : _isCredentialsAccepted = true;
            },
            [this](td::td_api::authorizationStateLoggingOut&) {
                qDebug() << "td::td_api::authorizationStateLoggingOut&";
                _isAuthCodeAccepted = false;
                _telegramCredentials.isEmpty() ? _isCredentialsAccepted = false : _isCredentialsAccepted = true;
            },
            [this](td::td_api::authorizationStateClosed&) {
                qDebug() << "td::td_api::authorizationStateClosed&";
                _isAuthCodeAccepted = false;
                _telegramCredentials.isEmpty() ? _isCredentialsAccepted = false : _isCredentialsAccepted = true;
            },
            [this](td::td_api::authorizationStateWaitPhoneNumber&) {
                qDebug() << "td::td_api::authorizationStateWaitPhoneNumber&";
                _telegramCredentials.isEmpty() ? _isCredentialsAccepted = false : _isCredentialsAccepted = true;

                sendQuery(
                    td::td_api::make_object<td::td_api::setAuthenticationPhoneNumber>(_telegramCredentials.phoneNumber, nullptr),
                    createAuthenticationQueryHandler()
                );
            },
            [this](td::td_api::authorizationStateWaitCode&) {
                qDebug() << "td::td_api::authorizationStateWaitCode&";
                _isAuthCodeAccepted = false;
                _telegramCredentials.isEmpty() ? _isCredentialsAccepted = false : _isCredentialsAccepted = true;

                if (_authorizationCode.empty())
                    return;

                qDebug() << "sended";

                sendQuery(
                    td::td_api::make_object<td::td_api::checkAuthenticationCode>(_authorizationCode),
                    createAuthenticationQueryHandler()
                );
            },
            [this](td::td_api::authorizationStateWaitTdlibParameters&) {
                qDebug() << "td::td_api::authorizationStateWaitTdlibParameters&";
                _telegramCredentials.isEmpty() ? _isCredentialsAccepted = false : _isCredentialsAccepted = true;

                if (_telegramCredentials.isEmpty())
                    return;

                auto request = td::td_api::make_object<td::td_api::setTdlibParameters>();

                request->api_id_ = _telegramCredentials.apiId;
                request->api_hash_ = _telegramCredentials.apiHash;
                
                request->database_directory_ = "tdlib";
                request->use_message_database_ = true;
                request->use_secret_chats_ = true;
                request->use_file_database_ = true;
                request->files_directory_ = "tdlib";

                request->system_language_code_ = "en";
                request->device_model_ = "Desktop";
                request->application_version_ = "1.0";

                qDebug() << "parameters sended";
                sendQuery(std::move(request), createAuthenticationQueryHandler());
            },
            [](auto& update) {}
        )
    );
}

void TelegramAuthorizer::checkAuthenticationError(Object object) {
    if (object->get_id() == td::td_api::error::ID) {
        // auto error = td::move_tl_object_as<td::td_api::error>(object);
        on_authorizationStateUpdate();
    }
}

std::uint64_t TelegramAuthorizer::nextQueryId() {
    return ++_currentQueryId;
}

bool TelegramAuthorizer::isCredentialsAccepted() const noexcept {
    return _isCredentialsAccepted;
}

bool TelegramAuthorizer::isAuthorized() const noexcept {
    return _isAuthCodeAccepted;
}
