#pragma once 


#include "../../core/Types.h"
#include "MediaCacheGeneral.h"

#include <qglobalstatic.h>

#include <QObject>
#include <QCache>

#include <chrono>


namespace Media {
    namespace {
        inline constexpr auto defaultCacheLimit = 1024 * 10; // 10 Mb
    } // namespace

    using namespace std::chrono_literals;

    class MediaCachePrivate: 
        public QObject,
        public QCache<Cache::Key,
            Cache::MediaCacheEntry>
    {
        Q_OBJECT
    public:
        MediaCachePrivate();
        ~MediaCachePrivate();

        void timerEvent(QTimerEvent*) override;
        bool insert(
            const QString& key,
            const OpenGL::Image& image,
            int cost);

        Cache::Key insert(
            const OpenGL::Image& image,
            int cost);

        bool remove(const QString& key);
        bool remove(const Cache::Key& key);

        void resizeKeyArray(int size);
        Cache::Key createKey();

        void releaseKey(Cache::Key& key);
        void clear();

        OpenGL::Image* object(const QString& key) const;
        OpenGL::Image* object(Cache::Key& key) const;

        static Cache::KeyData* get(Cache::Key& key);
        static Cache::KeyData* getKeyData(Cache::Key* key);

        bool flushDetachedPixmaps(bool nt);
    private:
        static constexpr auto soon_time = 10s;
        static constexpr auto flush_time = 30s;

        int* keyArray;
        int theid;

        int ps;
        int keyArraySize;

        int freeKey;

        QHash<QString, Media::Cache::Key> cacheKeys;
        bool t;
    };

    Q_GLOBAL_STATIC(MediaCachePrivate, pm_cache);
} // namespace Media