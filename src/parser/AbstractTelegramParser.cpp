#include "AbstractTelegramParser.h"

#include <cstdint>
#include <iostream>
#include <vector>

#include <QString>
#include <QStorageInfo>

#include <chrono>
#include <regex>

#include <QFile>
#include <QDir>


AbstractTelegramParser::AbstractTelegramParser() :
    _clientId(_clientManager->create_client_id())
    , _isCredentialsAccepted(false)
    , _isAuthCodeAccepted(false)
    , _isWaiting(false)
    , _currentQueryId(0)
    , _authenticationQueryId(0)
    , _userDataManager(std::make_unique<UserDataManager>())
    , _clientManager(std::make_unique<td::ClientManager>())
    , _thread(new QThread())
{
    if (_thread->isRunning())
        _thread->quit();

    moveToThread(_thread);
    _thread->start();

    td::ClientManager::execute(td::td_api::make_object<td::td_api::setLogVerbosityLevel>(0));
    sendQuery(td::td_api::make_object<td::td_api::getOption>("version"), {});

    const auto& largestDiskPath = findLargestNonSystemDisk();
    const auto tempDirectoryPath = largestDiskPath + PROJECT_NAME + "_temp";

    QDir dir(tempDirectoryPath);

    if (dir.exists(tempDirectoryPath) == false)
        dir.mkdir(tempDirectoryPath);

    dir.mkdir(tempDirectoryPath + "\\test");

    _databaseDirectory = dir.absolutePath().toStdString();
    _filesDirectory = dir.absolutePath().toStdString() + "\\test";
   
   /* _userDataManager->setDownloadSensitiveContentAbility(false);
    _userDataManager->setTargetChannels(QStringList({ "fksfhsfierw3irh", "antifishechki" }));
    _userDataManager->setLastPostsCountForChannels(3);*/

    authorizationCheck();
}


void AbstractTelegramParser::authorizationCheck() {
    if (_userDataManager->isTelegramCredentialsValid())
        setTelegramCredentials(_userDataManager->getTelegramCredentials());
    
    qDebug() << isCredentialsAccepted() << isAuthorized();
    if (isCredentialsAccepted() && isAuthorized())
        return;

    if (_authDialog == nullptr)
        _authDialog = std::make_unique<AuthenticationDialog>();

    if (isCredentialsAccepted() && isAuthorized() == false)
        _authDialog->toSecondFrame();

    _authDialog->show();

    connect(_authDialog.get(), &AuthenticationDialog::credentialsAccepted, this, &AbstractTelegramParser::setTelegramCredentials);
    connect(_authDialog.get(), &AuthenticationDialog::authCodeAccepted, this, &AbstractTelegramParser::setAuthorizationCode);
    connect(_authDialog.get(), &AuthenticationDialog::needSendCodeAgain, this, &AbstractTelegramParser::sendTelegramAuthCode);
}

void AbstractTelegramParser::setTelegramCredentials(const TelegramCredentials& credentials) {
    _telegramCredentials = credentials;

    for (;;) {
        processResponse(_clientManager->receive(10));

        if (_authorizationState == nullptr)
            continue;

        if (_authorizationState->get_id() == td::td_api::authorizationStateWaitCode::ID
            || _authorizationState->get_id() == td::td_api::authorizationStateReady::ID
            )
            return;
    }
}

void AbstractTelegramParser::setAuthorizationCode(const QString& code) {
    _authorizationCode = code.toStdString();

    for (;;) {
        processResponse(_clientManager->receive(10));

        if (_authorizationState == nullptr)
            continue;

        if (_authorizationState->get_id() == td::td_api::authorizationStateReady::ID)
            return;
    }
}

auto AbstractTelegramParser::createAuthenticationQueryHandler() {
    return [this, id = _authenticationQueryId](Object object) {
        if (id == _authenticationQueryId)
            checkAuthenticationError(std::move(object));
        };
}

bool AbstractTelegramParser::sendTelegramAuthCode() {
    if (_authorizationState->get_id() != td::td_api::authorizationStateWaitCode::ID) {
        _authDialog->shake();
        return false;
    }

    sendQuery(
        td::td_api::make_object<td::td_api::resendAuthenticationCode>(),
        createAuthenticationQueryHandler()
    );

    return true;
}

void AbstractTelegramParser::sendQuery(td::td_api::object_ptr<td::td_api::Function> f, std::function<void(Object)> handler) {
    const auto queryId = nextQueryId();

    if (handler)
        _handlers.emplace(queryId, std::move(handler));

    _clientManager->send(_clientId, queryId, std::move(f));
}

void AbstractTelegramParser::processResponse(td::ClientManager::Response response) {
    if (_isWaiting)
        return;

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

void AbstractTelegramParser::processUpdate(td::td_api::object_ptr<td::td_api::Object> update) {
    if (_isWaiting)
        return;

    td::td_api::downcast_call(
        *update, Telegram::overloaded(
            [this](td::td_api::updateAuthorizationState& update_authorization_state) {
                _authorizationState = std::move(update_authorization_state.authorization_state_);
                on_authorizationStateUpdate();
            },
            [this](td::td_api::updateConnectionState&) {
                on_authorizationStateUpdate();
            },
            [](auto& update) {

            }
        )
    );
}

void AbstractTelegramParser::on_authorizationStateUpdate() {
    _authenticationQueryId++;

    td::td_api::downcast_call(*_authorizationState,
        Telegram::overloaded(
            [this](td::td_api::authorizationStateReady&) {
                qDebug() << "authorizationStateReady";
                emit userAuthorized();

                _isAuthCodeAccepted = true;
                _telegramCredentials.isEmpty() ? _isCredentialsAccepted = false : _isCredentialsAccepted = true;

                if (_authDialog) {
                    _authDialog->setCloseAbility(true);
                }

                _userDataManager->setTelegramCredentials(_telegramCredentials);
            },
            [this](td::td_api::authorizationStateLoggingOut&) {
                _isAuthCodeAccepted = false;
                _telegramCredentials.isEmpty() ? _isCredentialsAccepted = false : _isCredentialsAccepted = true;
            },
            [this](td::td_api::authorizationStateClosed&) {
                _isAuthCodeAccepted = false;
                _telegramCredentials.isEmpty() ? _isCredentialsAccepted = false : _isCredentialsAccepted = true;
            },
            [this](td::td_api::authorizationStateWaitPhoneNumber&) {
                qDebug() << "authorizationStateWaitPhoneNumber";
                _telegramCredentials.isEmpty() ? _isCredentialsAccepted = false : _isCredentialsAccepted = true;

                sendQuery(
                    td::td_api::make_object<td::td_api::setAuthenticationPhoneNumber>(_telegramCredentials.phoneNumber, nullptr),
                    createAuthenticationQueryHandler()
                );
            },
            [this](td::td_api::authorizationStateWaitCode&) {
                qDebug() << "authorizationStateWaitCode";
                _isAuthCodeAccepted = false;
                _telegramCredentials.isEmpty() ? _isCredentialsAccepted = false : _isCredentialsAccepted = true;

                if (_authorizationCode.empty())
                    return;
                qDebug() << "send auth code: " << _authorizationCode;
                sendQuery(
                    td::td_api::make_object<td::td_api::checkAuthenticationCode>(_authorizationCode),
                    createAuthenticationQueryHandler()
                );

                _userDataManager->setTelegramCredentials(_telegramCredentials);
            },
            [this](td::td_api::authorizationStateWaitTdlibParameters&) {
                qDebug() << "authorizationStateWaitTdlibParameters";
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
                request->use_chat_info_database_ = true;

                request->use_secret_chats_ = true;
                request->use_file_database_ = true;
                request->use_test_dc_ = false;

                request->system_language_code_ = "en";
                request->device_model_ = "Desktop";
                request->system_version_ = "1.0";
                request->application_version_ = "1.0";

                sendQuery(
                    std::move(request),
                    createAuthenticationQueryHandler()
                );
            },
            [](auto& update) {}
        )
    );
}

void AbstractTelegramParser::checkAuthenticationError(Object object) {
    if (object->get_id() == td::td_api::error::ID) {
        auto error = td::move_tl_object_as<td::td_api::error>(object);
        std::cout << to_string(error) << error->code_ << std::endl;

        handleError(error);
        on_authorizationStateUpdate();
    }
}

void AbstractTelegramParser::handleTooManyRequestsError(const td::td_api::object_ptr<td::td_api::error>& error) {
    std::smatch match;
    int seconds = 0;

    if (std::regex_search(error->message_, match, std::regex("retry after (\\d+)")))
        seconds = std::stoi(match[1]);

    if (seconds != 0) {
        _isWaiting = true;

        _authDialog->setSleepSeconds(seconds);
        _authDialog->shake();

        std::this_thread::sleep_for(std::chrono::seconds(seconds));
    }

    _isWaiting = false;
}

void AbstractTelegramParser::handleError(const td::td_api::object_ptr<td::td_api::error>& error) {
    switch (error->code_) {
    case Telegram::ErrorCodes::TooManyRequests:
        _authDialog->setErrorCode(Telegram::ErrorCodes::TooManyRequests);
        handleTooManyRequestsError(error);

        _authDialog->repaint();
        break;

    case Telegram::ErrorCodes::IncorrectApiHashOrId:
        _authDialog->setErrorCode(Telegram::ErrorCodes::IncorrectApiHashOrId);
        _authDialog->shake();

        _authDialog->repaint();
        break;

    case Telegram::ErrorCodes::IncorrectAuthCode:
        _authDialog->setErrorCode(Telegram::ErrorCodes::IncorrectAuthCode);
        _authDialog->shake();

        _authDialog->repaint();

        _authorizationCode.erase(_authorizationCode.begin(), _authorizationCode.end());
        break;

    case Telegram::ErrorCodes::IncorrectPhoneNumber:
        _authDialog->setErrorCode(Telegram::ErrorCodes::IncorrectPhoneNumber);
        _authDialog->shake();

        _authDialog->repaint();
        break;
    }
}

QString AbstractTelegramParser::findLargestNonSystemDisk() const {
    qint64 maximumSize = 0;
    QString driverName = "";

    foreach(const auto& storage, QStorageInfo::mountedVolumes())
        if (storage.isValid() && storage.isReady())
            if (storage.isReadOnly() == false)
                if (storage.bytesFree() > maximumSize) {
                    maximumSize = storage.bytesFree();
                    driverName = storage.rootPath();
                }

    return driverName;
}


std::uint64_t AbstractTelegramParser::nextQueryId() {
    return ++_currentQueryId;
}

bool AbstractTelegramParser::isCredentialsAccepted() const noexcept {
    return _isCredentialsAccepted
        ? _userDataManager->isTelegramCredentialsValid()
        : _isAuthCodeAccepted;
}

bool AbstractTelegramParser::isAuthorized() const noexcept {
    return _isAuthCodeAccepted;
}
