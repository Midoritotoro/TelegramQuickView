#pragma once

#include "MediaCacheGeneral.h"
#include "MediaCachePrivate.h"

#include "../MediaItemBase.h"


namespace Media {
	class MediaCache {
	public:
        static int cacheLimit();
        static void setCacheLimit(int limit);

        static bool find(const QString& key, OpenGL::Image* image);
        static bool insert(const QString& key, const OpenGL::Image& image);

        static void remove(const QString& key);

        static void clear();
	};
} // namespace Media
