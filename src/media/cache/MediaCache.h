#pragma once

#include "MediaCacheGeneral.h"
#include "MediaCachePrivate.h"


namespace Media {
	template <class Key, class T>
	class MediaCache {
	public:
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
} // namespace Media
