#pragma once 

#include <QCache>
#include <qHash>

#include <QPixmap>

#include <private/qpixmap_raster_p.h>
#include <private/qimage_p.h>

#include "../../opengl/OpenGLImage.h"
#include <QPaintEngine>


namespace Media::Cache {
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

        [[nodiscard]] inline KeyData* data_ptr() {
            Expects(d != nullptr);
            return d;
        }

        void setData(KeyData* data) {
            d = data;
        }

    private:
        friend size_t qHash(const Key& k, size_t seed = 0) noexcept
        {
            return k.hash(seed);
        }

        size_t hash(size_t seed) const noexcept;

        KeyData* d;
    };

    class MediaCacheEntry : public OpenGL::Image
    {
    public:
        MediaCacheEntry(
            const Key& key,
            const OpenGL::Image& image);
        ~MediaCacheEntry();

        Key key;
    };
} // namespace Media::Cache