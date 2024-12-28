#pragma once

#include <numeric>
#include <QObject>

#include <QPixmap>
#include <QThread>

#include <QCoreApplication>


namespace Media::Cache {
    static inline qsizetype PixmapCost(const QPixmap& pixmap)
    {
        const qint64 costKb = static_cast<qint64>(pixmap.width())
            * pixmap.height() * pixmap.depth() / (8 * 1024);
        const qint64 costMax = std::numeric_limits<qsizetype>::max();

        return static_cast<qsizetype>(qBound(1LL, costKb, costMax));
    }

    static inline bool PixmapCacheThreadTest()
    {
        if (Q_LIKELY(QCoreApplication::instance() && QThread::currentThread() == QCoreApplication::instance()->thread()))
            return true;

        return false;
    }

    Q_AUTOTEST_EXPORT void PixmapCacheFlushDetachedPixmaps()
    {
        if (!PixmapCacheThreadTest())
            return;

        pm_cache()->flushDetachedPixmaps(true);
    }

    Q_AUTOTEST_EXPORT int PixmapCacheTotalUsed()
    {
        if (!PixmapCacheThreadTest())
            return 0;

        return (pm_cache()->totalCost() + 1023) / 1024;
    }
} // namespace Media::Cache