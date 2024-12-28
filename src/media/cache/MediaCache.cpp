#include "MediaCache.h"
#include "MediaCacheUtility.h"


namespace Media {
    bool MediaCache::find(const QString& key, OpenGL::Image* image)
    {
        if (key.isEmpty() || !Cache::PixmapCacheThreadTest())
            return false;

        OpenGL::Image* ptr = pm_cache()->object(key);
        if (ptr && image)
            *image = *ptr;

        return (ptr != nullptr);
    }

    bool MediaCache::insert(const QString& key, const OpenGL::Image& image)
    {
        if (key.isEmpty() || !Cache::PixmapCacheThreadTest())
            return false;

        return pm_cache()->insert(key, image, Cache::PixmapCost(image.toPixmap()));
    }

    int MediaCache::cacheLimit()
    {
        if (!Cache::PixmapCacheThreadTest())
            return 0;

        return pm_cache()->maxCost();
    }

    void MediaCache::setCacheLimit(int n)
    {
        if (!Cache::PixmapCacheThreadTest())
            return;

        pm_cache()->setMaxCost(n);
    }

    void MediaCache::remove(const QString& key)
    {
        if (key.isEmpty() || !Cache::PixmapCacheThreadTest())
            return;

        pm_cache()->remove(key);
    }

    void MediaCache::clear()
    {
        if (!QCoreApplication::closingDown() && !Cache::PixmapCacheThreadTest())
            return;
        try {
            if (pm_cache.exists())
                pm_cache->clear();
        } catch (const std::bad_alloc&) {
           
        }
    }
} // namespace Media
