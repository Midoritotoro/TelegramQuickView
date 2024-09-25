#pragma once

#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>

#include <map>
#include <memory>
#include <sstream>
#include <string>

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
private:
    using Object = td::td_api::object_ptr<td::td_api::Object>;
    std::unique_ptr<td::ClientManager> client_manager_;
    std::int32_t client_id_{ 0 };

    td::td_api::object_ptr<td::td_api::AuthorizationState> authorization_state_;

    bool are_authorized_{ false };
    bool need_restart_{ false };
    bool _isCredentialsAccepted{ false };
    bool _isAuthCodeAccepted{ false };

    std::uint64_t current_query_id_{ 0 };
    std::uint64_t authentication_query_id_{ 0 };

    std::map<std::uint64_t, std::function<void(Object)>> handlers_;

    TelegramCredentials _telegramCredentials;
    std::string _authorizationCode;

public:
    TelegramAuthorizer();

    void setTelegramCredentials(const TelegramCredentials& credentials);
    void setAuthorizationCode(std::string code);

    [[nodiscard]] bool isCredentialsAccepted();
    [[nodiscard]] bool isAuthorized();

    TelegramAuthorizer& operator=(const TelegramAuthorizer& instance) {
        return *this;
    }
private:
    void restart();

    void send_query(td::td_api::object_ptr<td::td_api::Function> f, std::function<void(Object)> handler);
    void process_response(td::ClientManager::Response response);

    void process_update(td::td_api::object_ptr<td::td_api::Object> update);

    auto create_authentication_query_handler();
    void on_authorization_state_update();

    void check_authentication_error(Object object);

    std::uint64_t next_query_id();
};
