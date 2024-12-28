#include "MediaCachePrivate.h"

#include <QThread>
#include <QObject>

#include <qDebug>
#include <QCoreApplication>


namespace Media {
    MediaCachePrivate::MediaCachePrivate():
        QObject(nullptr),
        QCache<Media::Cache::Key, Media::Cache::MediaCacheEntry>(defaultCacheLimit),
        keyArray(nullptr), theid(0), ps(0), keyArraySize(0), freeKey(0), t(false)
    {}

    MediaCachePrivate::~MediaCachePrivate()
    {
        clear();
        free(keyArray);
    }

    bool MediaCachePrivate::flushDetachedPixmaps(bool nt)
    {
        auto mc = maxCost();
        const qsizetype currentTotal = totalCost();
        const qsizetype oldSize = size();
        if (currentTotal)
            setMaxCost(nt ? currentTotal * 3 / 4 : currentTotal - 1);
        setMaxCost(mc);
        ps = totalCost();
        return size() != oldSize;
    }

    void MediaCachePrivate::timerEvent(QTimerEvent*)
    {
        bool nt = totalCost() == ps;
        if (!flushDetachedPixmaps(nt)) {
            killTimer(theid);
            theid = 0;
        }
        else if (nt != t) {
            killTimer(theid);
            theid = startTimer(nt ? soon_time : flush_time);
            t = nt;
        }
    }


    OpenGL::Image* MediaCachePrivate::object(const QString& key) const
    {
        if (const auto it = cacheKeys.find(key); it != cacheKeys.cend())
            return object(const_cast<Cache::Key&>(it.value()));
        return nullptr;
    }

    OpenGL::Image* MediaCachePrivate::object(Cache::Key& key) const
    {
        Q_ASSERT(key.isValid());
        OpenGL::Image* ptr = QCache<Cache::Key, Cache::MediaCacheEntry>::object(key);
        //We didn't find the pixmap in the cache, the key is not valid anymore
        if (!ptr)
            const_cast<MediaCachePrivate*>(this)->releaseKey(key);
        return ptr;
    }

    bool MediaCachePrivate::insert(const QString& key, const OpenGL::Image& image, int cost)
    {
        remove(key);

        auto k = insert(image, cost);
        if (k.isValid()) {
            k.data_ptr()->stringKey = key;
            cacheKeys[key] = std::move(k);
            return true;
        }
        return false;
    }

    Cache::Key MediaCachePrivate::insert(const OpenGL::Image& image, int cost)
    {
        Cache::Key cacheKey = createKey();
        bool success = QCache<Cache::Key, Cache::MediaCacheEntry>::insert(cacheKey, new Cache::MediaCacheEntry(cacheKey, image), cost);
        Q_ASSERT(success || !cacheKey.isValid());
        if (success) {
            if (!theid) {
                theid = startTimer(flush_time);
                t = false;
            }
        }
        return cacheKey;
    }

    bool MediaCachePrivate::remove(const QString& key)
    {
        const auto cacheKey = cacheKeys.take(key);
        return cacheKey.isValid() && remove(cacheKey);
    }

    bool MediaCachePrivate::remove(const Cache::Key& key)
    {
        return QCache<Cache::Key, Cache::MediaCacheEntry>::remove(key);
    }

    void MediaCachePrivate::resizeKeyArray(int size)
    {
        if (size <= keyArraySize || size == 0)
            return;
        keyArray = q_check_ptr(static_cast<int*>(realloc(keyArray,
            size * sizeof(int))));
        for (int i = keyArraySize; i != size; ++i)
            keyArray[i] = i + 1;
        keyArraySize = size;
    }

    Cache::Key MediaCachePrivate::createKey()
    {
        if (freeKey == keyArraySize)
            resizeKeyArray(keyArraySize ? keyArraySize << 1 : 2);
        int id = freeKey;
        freeKey = keyArray[id];
        Cache::Key key;
        Cache::KeyData* d = MediaCachePrivate::getKeyData(&key);
        d->key = ++id;
        return key;
    }

    void MediaCachePrivate::releaseKey(Cache::Key& key)
    {
        Cache::KeyData* keyData = key.data_ptr();
        if (!keyData)
            return;
        if (!keyData->stringKey.isNull())
            cacheKeys.remove(keyData->stringKey);
        if (keyData->key > keyArraySize || keyData->key <= 0)
            return;
        keyData->key--;
        keyArray[keyData->key] = freeKey;
        freeKey = keyData->key;
        keyData->isValid = false;
        keyData->key = 0;
    }

    void MediaCachePrivate::clear()
    {
        free(keyArray);
        keyArray = nullptr;
        freeKey = 0;
        keyArraySize = 0;
        //Mark all keys as invalid
        QList<Cache::Key> keys = QCache<Cache::Key, Cache::MediaCacheEntry>::keys();
        for (auto& key : keys) {
            if (key.data_ptr())
                key.data_ptr()->isValid = false;
        }
        QCache<Cache::Key, Cache::MediaCacheEntry>::clear();
        // Nothing left to flush; stop the timer
        if (theid) {
            killTimer(theid);
            theid = 0;
        }
    }

    Cache::KeyData* MediaCachePrivate::get(Cache::Key& key) {
        return key.data_ptr();
    }

    Cache::KeyData* MediaCachePrivate::getKeyData(Cache::Key* key)
    {
        if (!key->data_ptr())
            key->setData(new Cache::KeyData);
        return key->data_ptr();
    }

}