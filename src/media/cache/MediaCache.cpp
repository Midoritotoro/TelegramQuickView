#include "MediaCache.h"


namespace Media {
    

    Q_GLOBAL_STATIC(QPMCache, pm_cache)

        int Q_AUTOTEST_EXPORT q_QPixmapCache_keyHashSize()
    {
        return pm_cache()->size();
    }

    QPixmapCacheEntry::~QPixmapCacheEntry()
    {
        pm_cache()->releaseKey(key);
    }

    bool QPixmapCache::find(const QString& key, QPixmap* pixmap)
    {
        if (key.isEmpty() || !qt_pixmapcache_thread_test())
            return false;
        QPixmap* ptr = pm_cache()->object(key);
        if (ptr && pixmap)
            *pixmap = *ptr;
        return ptr != nullptr;
    }

    bool QPixmapCache::find(const Key& key, QPixmap* pixmap)
    {
        if (!qt_pixmapcache_thread_test())
            return false;
        //The key is not valid anymore, a flush happened before probably
        if (!key.d || !key.d->isValid)
            return false;
        QPixmap* ptr = pm_cache()->object(key);
        if (ptr && pixmap)
            *pixmap = *ptr;
        return ptr != nullptr;
    }

    bool QPixmapCache::insert(const QString& key, const QPixmap& pixmap)
    {
        if (key.isEmpty() || !qt_pixmapcache_thread_test())
            return false;
        return pm_cache()->insert(key, pixmap, cost(pixmap));
    }

    QPixmapCache::Key QPixmapCache::insert(const QPixmap& pixmap)
    {
        if (!qt_pixmapcache_thread_test())
            return QPixmapCache::Key();
        return pm_cache()->insert(pixmap, cost(pixmap));
    }

    int QPixmapCache::cacheLimit()
    {
        if (!qt_pixmapcache_thread_test())
            return 0;
        return pm_cache()->maxCost();
    }

    void QPixmapCache::setCacheLimit(int n)
    {
        if (!qt_pixmapcache_thread_test())
            return;
        pm_cache()->setMaxCost(n);
    }

    void QPixmapCache::remove(const QString& key)
    {
        if (key.isEmpty() || !qt_pixmapcache_thread_test())
            return;
        pm_cache()->remove(key);
    }

    void QPixmapCache::remove(const Key& key)
    {
        if (!qt_pixmapcache_thread_test())
            return;

        if (!key.d || !key.d->isValid)
            return;
        pm_cache()->remove(key);
    }

    void QPixmapCache::clear()
    {
        if (!QCoreApplication::closingDown() && !qt_pixmapcache_thread_test())
            return;
        QT_TRY{
            if (pm_cache.exists())
                pm_cache->clear();
        } QT_CATCH(const std::bad_alloc&) {
           
        }
    }

    Q_AUTOTEST_EXPORT void qt_qpixmapcache_flush_detached_pixmaps() // for tst_qpixmapcache
    {
        if (!qt_pixmapcache_thread_test())
            return;
        pm_cache()->flushDetachedPixmaps(true);
    }

    Q_AUTOTEST_EXPORT int qt_qpixmapcache_qpixmapcache_total_used() // for tst_qpixmapcache
    {
        if (!qt_pixmapcache_thread_test())
            return 0;
        return (pm_cache()->totalCost() + 1023) / 1024;
    }
} // namespace Media
