#include "MediaCachePrivate.h"

#include <QThread>
#include <QObject>

#include <qDebug>
#include <QCoreApplication>


namespace Media {
    namespace {
        inline constexpr auto defaultCacheLimit = 1024 * 10; // 10 Mb
    } // namespace

    using namespace std::chrono_literals;

    static inline qsizetype cost(const QPixmap& pixmap)
    {
        const qint64 costKb = static_cast<qint64>(pixmap.width())
            * pixmap.height() * pixmap.depth() / (8 * 1024);
        const qint64 costMax = std::numeric_limits<qsizetype>::max();

        return static_cast<qsizetype>(qBound(1LL, costKb, costMax));
    }

    static inline bool qt_pixmapcache_thread_test()
    {
        if (Q_LIKELY(QCoreApplication::instance() && QThread::currentThread() == QCoreApplication::instance()->thread()))
            return true;

        return false;
    }

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


    QPixmap* MediaCachePrivate::object(const QString& key) const
    {
        if (const auto it = cacheKeys.find(key); it != cacheKeys.cend())
            return object(it.value());
        return nullptr;
    }

    QPixmap* MediaCachePrivate::object(const Cache::Key& key) const
    {
        Q_ASSERT(key.isValid());
        QPixmap* ptr = QCache<Cache::Key, Cache::MediaCacheEntry>::object(key);
        //We didn't find the pixmap in the cache, the key is not valid anymore
        if (!ptr)
            const_cast<QPMCache*>(this)->releaseKey(key);
        return ptr;
    }

    bool MediaCachePrivate::insert(const QString& key, const QPixmap& pixmap, int cost)
    {
        remove(key);

        auto k = insert(pixmap, cost);
        if (k.isValid()) {
            k.d->stringKey = key;
            cacheKeys[key] = std::move(k);
            return true;
        }
        return false;
    }

    Cache::Key MediaCachePrivate::insert(const QPixmap& pixmap, int cost)
    {
        Cache::Key cacheKey = createKey();
        bool success = QCache<Cache::Key, Cache::MediaCacheEntry>::insert(cacheKey, new Cache::MediaCacheEntry(cacheKey, pixmap), cost);
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

    void MediaCachePrivate::releaseKey(const Cache::Key& key)
    {
        Cache::KeyData* keyData = key.d;
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
        const QList<Cache::Key> keys = QCache<Cache::Key, MediaCacheEntry>::keys();
        for (const auto& key : keys) {
            if (key.d)
                key.d->isValid = false;
        }
        QCache<QPixmapCache::Key, QPixmapCacheEntry>::clear();
        // Nothing left to flush; stop the timer
        if (theid) {
            killTimer(theid);
            theid = 0;
        }
    }

    Cache::KeyData* MediaCachePrivate::get(const Cache::Key& key) {
        return key.d;
    }

    Cache::KeyData* MediaCachePrivate::getKeyData(Cache::Key* key)
    {
        if (!key->d)
            key->d = new Cache::KeyData;
        return key->d;
    }

}