#include "TelegramAuthorizer.h"

#include <cstdint>
#include <iostream>
#include <vector>
#include <future>

#include <QString>
#include <QDebug>


TelegramAuthorizer::TelegramAuthorizer() :
      _clientId(0)
    , _isCredentialsAccepted(false)
    , _isAuthCodeAccepted(false)
    , _currentQueryId(0)
    , _authenticationQueryId(0)
    
{
    td::ClientManager::execute(td::td_api::make_object<td::td_api::setLogVerbosityLevel>(1));

    _clientManager = std::make_unique<td::ClientManager>();
    _clientId = _clientManager->create_client_id();

    sendQuery(td::td_api::make_object<td::td_api::getOption>("version"), {});
}

void TelegramAuthorizer::setTelegramCredentials(const TelegramCredentials& credentials) {
    _telegramCredentials = credentials;

    while (true) {
        processResponse(_clientManager->receive(100));
        qDebug() << "setCred";

        if (_authorizationState.get() == nullptr)
            continue;
   
        if (_authorizationState.get()->get_id() == td::td_api::authorizationStateWaitCode::ID
            || _authorizationState.get()->get_id() == td::td_api::authorizationStateReady::ID
            ) {
            qDebug() << "cred returned";
            return;
        }
    }
}

void TelegramAuthorizer::setAuthorizationCode(std::string code) {
    _authorizationCode = code;

    while (true) {
        processResponse(_clientManager->receive(100));

        if (_authorizationState.get() == nullptr)
            continue;

        if (_authorizationState.get()->get_id() == td::td_api::authorizationStateReady::ID
            || _authorizationCode.empty())
            return;
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
    auto future = std::async(std::launch::async, [this, _update = update.get()]() {
        td::td_api::downcast_call(
            *_update, overloaded(
                [this](td::td_api::updateAuthorizationState& update_authorization_state) {
                    _authorizationState = std::move(update_authorization_state.authorization_state_);
                },
                [](auto& update) {}
            )
        );
        }
    );
}

auto TelegramAuthorizer::createAuthenticationQueryHandler() {
    return [this, id = _authenticationQueryId](Object object) {
        if (id == _authenticationQueryId)
            checkAuthenticationError(std::move(object));
    };
}

void TelegramAuthorizer::on_authorizationStateUpdate() {
    _authenticationQueryId++;

    td::td_api::downcast_call(*_authorizationState,
        overloaded(
            [this](td::td_api::authorizationStateReady&) {
                _isAuthCodeAccepted = true;
                _isCredentialsAccepted = true;
            },
            [this](td::td_api::authorizationStateLoggingOut&) {
                _isAuthCodeAccepted = false;
                _isCredentialsAccepted = false;
            },
            [this](td::td_api::authorizationStateClosed&) {
                _isAuthCodeAccepted = false;
                _isCredentialsAccepted = false;
            },
            [this](td::td_api::authorizationStateWaitPhoneNumber&) {
                _isCredentialsAccepted = false;

                sendQuery(
                    td::td_api::make_object<td::td_api::setAuthenticationPhoneNumber>(_telegramCredentials.phoneNumber, nullptr),
                    createAuthenticationQueryHandler()
                );
            },
            [this](td::td_api::authorizationStateWaitCode&) {
                _isAuthCodeAccepted = false;
                _isCredentialsAccepted = true;
                qDebug() << "waitCodeState" << _authorizationCode;

              /*  while (_authorizationCode.empty()) {
                    qDebug() << "wait";
                    _sleep(100);
                }*/
                if (_authorizationCode.empty())
                    return;
                qDebug() << "sended";

                sendQuery(
                    td::td_api::make_object<td::td_api::checkAuthenticationCode>(_authorizationCode),
                    createAuthenticationQueryHandler()
                );

                _authorizationCode.erase();
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
                request->use_file_database_ = true;
                request->database_encryption_key_ = td::td_api::make_object<td::td_api::setDatabaseEncryptionKey>().get()->new_encryption_key_;
                request->files_directory_ = "tdlib/files";
                request->use_test_dc_ = true;

                request->system_language_code_ = "en";
                request->device_model_ = "Desktop";
                request->application_version_ = "1.0";

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
