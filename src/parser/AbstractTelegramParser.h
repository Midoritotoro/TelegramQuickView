#pragma once

#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>

#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <future>

#include "AuthenticationDialog.h"


class AbstractTelegramParser: public QObject
{
    Q_OBJECT
protected:
    using Object = td::td_api::object_ptr<td::td_api::Object>;

    std::unique_ptr<td::ClientManager> _clientManager = nullptr;
    std::int32_t _clientId;

    td::td_api::object_ptr<td::td_api::AuthorizationState> _authorizationState;
    std::map<std::uint64_t, std::function<void(Object)>> _handlers;

    std::unique_ptr<UserDataManager> _userDataManager = nullptr;

    bool _isWaiting;
private:
    bool _isCredentialsAccepted, _isAuthCodeAccepted;

    std::uint64_t _currentQueryId, _authenticationQueryId;

    TelegramCredentials _telegramCredentials;
    std::string _authorizationCode, _databaseDirectory, _filesDirectory;

    std::unique_ptr<AuthenticationDialog> _authDialog = nullptr;
public:
    AbstractTelegramParser();

    void setTelegramCredentials(const TelegramCredentials& credentials);
    void setAuthorizationCode(const QString& code);

    bool sendTelegramAuthCode();

    [[nodiscard]] bool isCredentialsAccepted() const noexcept;
    [[nodiscard]] bool isAuthorized() const noexcept;

    inline AbstractTelegramParser& operator=(const AbstractTelegramParser&) {
        return *this;
    }
Q_SIGNALS:
    void userAuthorized();
protected:
    void authorizationCheck();

    virtual void processResponse(td::ClientManager::Response response);
    void processUpdate(td::td_api::object_ptr<td::td_api::Object> update);

    void sendQuery(td::td_api::object_ptr<td::td_api::Function> f, std::function<void(Object)> handler);

    auto createAuthenticationQueryHandler();
    void on_authorizationStateUpdate();

    void checkAuthenticationError(Object object);

    std::uint64_t nextQueryId();

    [[nodiscard]] QString findLargestNonSystemDisk() const;

    void handleTooManyRequestsError(const td::td_api::object_ptr<td::td_api::error>& error);
    void handleError(const td::td_api::object_ptr<td::td_api::error>& error);
};
