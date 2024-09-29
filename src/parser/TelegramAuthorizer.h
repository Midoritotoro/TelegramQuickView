﻿#pragma once

#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>

#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <future>

#include <functional>

typedef struct _TelegramCredentials {
    int apiId = 0;
    std::string apiHash = "";
    std::string phoneNumber = "";

    [[nodiscard]] bool isEmpty() {
        return apiId == 0 && apiHash.empty() && phoneNumber.empty();
    }
} TelegramCredentials;


namespace detail {
    template <class... Fs>
    struct overload;

    template <class F>
    struct overload<F> : public F {
        explicit overload(F f) : F(f) {
        }
    };

    template <class F, class... Fs>

    struct overload<F, Fs...>
        : public overload<F>
        , public overload<Fs...> {
        overload(F f, Fs... fs) : overload<F>(f), overload<Fs...>(fs...) {
        }

        using overload<F>::operator();
        using overload<Fs...>::operator();
    };
}  // namespace detail


template <class... F>

auto overloaded(F... f) {
    return detail::overload<F...>(f...);
}


class TelegramAuthorizer
{
protected:
    using Object = td::td_api::object_ptr<td::td_api::Object>;

    std::unique_ptr<td::ClientManager> _clientManager;
    std::int32_t _clientId;

    td::td_api::object_ptr<td::td_api::AuthorizationState> _authorizationState;
private:
    bool _isCredentialsAccepted, _isAuthCodeAccepted;

    std::uint64_t _currentQueryId, _authenticationQueryId;

    std::map<std::uint64_t, std::function<void(Object)>> _handlers;

    TelegramCredentials _telegramCredentials;
    std::string _authorizationCode;
public:
    TelegramAuthorizer();

    void setTelegramCredentials(const TelegramCredentials& credentials);
    void setAuthorizationCode(std::string code);

    void sendTelegramAuthCode();

    [[nodiscard]] bool isCredentialsAccepted() const noexcept;
    [[nodiscard]] bool isAuthorized() const noexcept;

    inline TelegramAuthorizer& operator=(const TelegramAuthorizer&) {
        return *this;
    }
private:
    void sendQuery(td::td_api::object_ptr<td::td_api::Function> f, std::function<void(Object)> handler);
    void processResponse(td::ClientManager::Response response);

    void processUpdate(td::td_api::object_ptr<td::td_api::Object> update);

    auto createAuthenticationQueryHandler();
    void on_authorizationStateUpdate();

    void checkAuthenticationError(Object object);

    std::uint64_t nextQueryId();
};
