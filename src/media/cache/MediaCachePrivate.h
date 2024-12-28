#pragma once 


#include "../../core/Types.h"
#include "MediaCacheGeneral.h"

#include <QObject>
#include <QCache>

#include <chrono>


namespace Media {
    class MediaCachePrivate: 
        public QObject,
        public QCache<Media::Cache::Key,
            Media::Cache::MediaCacheEntry>
    {
        Q_OBJECT
    public:
        MediaCachePrivate();
        ~MediaCachePrivate();

        void timerEvent(QTimerEvent*) override;
        bool insert(
            const QString& key,
            const QPixmap& pixmap,
            int cost);

        Cache::Key insert(
            const QPixmap& pixmap,
            int cost);

        bool remove(const QString& key);
        bool remove(const Cache::Key& key);

        void resizeKeyArray(int size);
        Cache::Key createKey();

        void releaseKey(const Cache::Key& key);
        void clear();

        QPixmap* object(const QString& key) const;
        QPixmap* object(const Cache::Key& key) const;

        static Cache::KeyData* get(const Cache::Key& key);
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
} // namespace Media