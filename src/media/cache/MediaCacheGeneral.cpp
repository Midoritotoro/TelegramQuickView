#include "MediaCacheGeneral.h"


namespace Media::Cache {
    KeyData::KeyData():
        isValid(true), key(0), ref(1) {}
    KeyData::KeyData(const KeyData& other) :
        isValid(other.isValid), key(other.key), ref(1) {}

    KeyData::~KeyData()
    {}

    MediaCacheEntry::MediaCacheEntry(
        const Key& key,
        const gl::Image& image
    ): 
        gl::Image(image),
        key(key)
    {
        QPlatformPixmap* pd = handle();
        if (pd && pd->classId() == QPlatformPixmap::RasterClass) {
            QRasterPlatformPixmap* d = static_cast<QRasterPlatformPixmap*>(pd);
            if (!d->buffer()->isNull() && d->buffer()->data_ptr()->paintEngine
                && !d->buffer()->data_ptr()->paintEngine->isActive())
            {
                delete d->buffer()->data_ptr()->paintEngine;
                delete d->buffer()->data_ptr()->paintEngine;
                d->buffer()->data_ptr()->paintEngine = nullptr;
            }
        }
    }

 
    Key::Key():
        d(nullptr)
    {}


    Key::Key(const Key& other) {
        if (other.d)
            ++(other.d->ref);
        d = other.d;
    }

    Key::~Key() {
        if (d && --(d->ref) == 0)
            delete d;
    }

    bool Key::operator ==(const Key& key) const {
        return (d == key.d);
    }

    bool Key::isValid() const noexcept {
        return d && d->isValid;
    }

    Key& Key::operator =(const Key& other) {
        if (d != other.d) {
            if (other.d)
                ++(other.d->ref);
            if (d && --(d->ref) == 0)
                delete d;
            d = other.d;
        }
        return *this;
    }

    size_t Key::hash(size_t seed) const noexcept {
        return qHash(this->d ? this->d->key : 0, seed);
    }
}