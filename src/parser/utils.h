#pragma once

#include <functional>
#include <QString>

namespace Telegram {
    enum ErrorCodes {
        OK = 200,
        TooManyRequests = 429,
        IncorrectAuthCode = 400,
        IncorrectApiHashOrId = 1,
        IncorrectPhoneNumber = 2
    };

    struct Message {
        QString sender = "";
        QString text = "";
        QString date = "";

        QStringList attachments = {};
        int64_t mediaAlbumId = 0;

        [[nodiscard]] bool isNull() const {
            return text.isEmpty() && attachments.isEmpty();
        }
    };

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
} // namespace Telegram