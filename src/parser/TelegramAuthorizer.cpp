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
    td::ClientManager::execute(td::td_api::make_object<td::td_api::setLogVerbosityLevel>(0));

    _clientManager = std::make_unique<td::ClientManager>();
    _clientId = _clientManager->create_client_id();

    sendQuery(td::td_api::make_object<td::td_api::getOption>("version"), {});

    _userDataManager = std::make_unique<UserDataManager>();

    if (_userDataManager->isTelegramCredentialsValid())
        setTelegramCredentials(_userDataManager->getTelegramCredentials());

    if (isCredentialsAccepted() && isAuthorized())
        return;

    _authDialog = std::make_unique<AuthenticationDialog>();

    if (isCredentialsAccepted() && isAuthorized() == false)
        _authDialog->toSecondFrame();

    _authDialog->show();

    connect(_authDialog.get(), &AuthenticationDialog::credentialsAccepted, [this](const TelegramCredentials& credentials) {
        setTelegramCredentials(credentials);
    });

    connect(_authDialog.get(), &AuthenticationDialog::authCodeAccepted, [this](const QString& code) {
        qDebug() << "AuthCode: " << code;
        setAuthorizationCode(code.toStdString());
       });

    connect(_authDialog.get(), &AuthenticationDialog::needSendCodeAgain, [this]() {
        if (sendTelegramAuthCode() == false)
            _authDialog->shake();
        });
}

void TelegramAuthorizer::setTelegramCredentials(const TelegramCredentials& credentials) {
    _telegramCredentials = credentials;

    while (true) {
        processResponse(_clientManager->receive(10));

        if (_authorizationState == nullptr)
            continue;
   
        if (_authorizationState->get_id() == td::td_api::authorizationStateWaitCode::ID
            || _authorizationState->get_id() == td::td_api::authorizationStateReady::ID
        )
            return;
    }
}

void TelegramAuthorizer::setAuthorizationCode(std::string code) {
    _authorizationCode = code;

    while (true) {
        processResponse(_clientManager->receive(10));

        if (_authorizationState == nullptr)
            continue;

        if (_authorizationState->get_id() == td::td_api::authorizationStateReady::ID) 
            return;    
    }
}

auto TelegramAuthorizer::createAuthenticationQueryHandler() {
    return [this, id = _authenticationQueryId](Object object) {
        if (id == _authenticationQueryId)
            checkAuthenticationError(std::move(object));
        };
}

bool TelegramAuthorizer::sendTelegramAuthCode() {
    if (_authorizationState->get_id() != td::td_api::authorizationStateWaitCode::ID)
        return false;

    sendQuery(
        td::td_api::make_object<td::td_api::resendAuthenticationCode>(),
        createAuthenticationQueryHandler()
    );

    return true;
}

void TelegramAuthorizer::setDatabaseDirectory(std::string path) {
    _databaseDirectory = path;
}

void TelegramAuthorizer::setFilesDirectory(std::string path) {
    _filesDirectory = path;
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
    td::td_api::downcast_call(
        *update, overloaded(
            [this](td::td_api::updateAuthorizationState& update_authorization_state) {
                _authorizationState = std::move(update_authorization_state.authorization_state_);
                on_authorizationStateUpdate();
            },
            [this](td::td_api::updateConnectionState&) {
                on_authorizationStateUpdate();
            },
            [](auto& update) {}
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

                qDebug() << "codeSended: " << _authorizationCode;
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
                
                if (_databaseDirectory.empty() == false)
                    request->database_directory_ = _databaseDirectory;

                if (_filesDirectory.empty() == false)
                    request->files_directory_ = _filesDirectory;

                request->use_message_database_ = true;
                request->use_secret_chats_ = true;
                request->use_file_database_ = true;
                request->use_test_dc_ = false;

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
        auto error = td::move_tl_object_as<td::td_api::error>(object);
        std::cout << "Error: " << to_string(error) << std::flush;
        on_authorizationStateUpdate();
    }
}

std::uint64_t TelegramAuthorizer::nextQueryId() {
    return ++_currentQueryId;
}

bool TelegramAuthorizer::isCredentialsAccepted() const noexcept {
    if (_isCredentialsAccepted == false)
        return _isAuthCodeAccepted;
    return _userDataManager->isTelegramCredentialsValid();
}

bool TelegramAuthorizer::isAuthorized() const noexcept {
    if (_isAuthCodeAccepted == false)
        return _isAuthCodeAccepted;
    return _userDataManager->isTelegramAuthCodeValid();
}
