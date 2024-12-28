#pragma once 

#include <QCache>
#include <qHash>

#include <QPixmap>

#include <private/qpixmap_raster_p.h>
#include <private/qimage_p.h>

#include "../../gl/GlImage.h"

#include <QPaintEngine>


namespace Media::Cache {
    class Key;
    class KeyData {
    public:
        KeyData();
        KeyData(const KeyData& other);

        ~KeyData();

        QString stringKey;
        bool isValid;

        int key;
        int ref;
    };

    class MediaCacheEntry : public gl::Image
    {
    public:
        MediaCacheEntry(
            const Key& key,
            const gl::Image &image);
        ~MediaCacheEntry();

        Key key;
    };

        class Key
        {
        public:
            Key();
            Key(const Key& other);

            Key(Key&& other) noexcept : d(other.d) { other.d = nullptr; }
            Key& operator =(Key&& other) noexcept { swap(other); return *this; }

            ~Key();

            bool operator ==(const Key& key) const;
            inline bool operator !=(const Key& key) const {
                return !operator==(key);
            }

            Key& operator =(const Key& other);

            void swap(Key& other) noexcept { qt_ptr_swap(d, other.d); }
            bool isValid() const noexcept;

        private:
            friend size_t qHash(const Key& k, size_t seed = 0) noexcept
            {
                return k.hash(seed);
            }

            size_t hash(size_t seed) const noexcept;

            KeyData* d;

            friend class MediaCache;
            friend class MediaCachePrivate;
        };

        static int cacheLimit();
        static void setCacheLimit(int);

        static bool find(const QString& key, QPixmap* pixmap);
        static bool find(const Key& key, QPixmap* pixmap);

        static bool insert(const QString& key, const QPixmap& pixmap);
        static Key insert(const QPixmap& pixmap);

        static void remove(const QString& key);
        static void remove(const Key& key);
        static void clear();
    };
} // namespace Media::Cache