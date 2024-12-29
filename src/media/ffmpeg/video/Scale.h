#include "Types.h"
#include <QImage>


namespace FFmpeg {
	[[nodiscard]] int GetCpuCount();

	void Clean(filter_t* p_filter);
	[[nodiscard]] int Init(filter_t* p_filter);
} // namespace FFmpeg