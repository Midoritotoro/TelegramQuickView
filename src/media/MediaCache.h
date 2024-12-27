#pragma once

#include <QCache>


namespace Media {
	template <class Key, class T>
	class MediaCache: public QCache<Key, T> {
		
	};
} // namespace Media