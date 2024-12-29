#include "Types.h"
#include <QImage>


namespace FFmpeg {
	[[nodiscard]] int GetCpuCount();

	void Clean(filter_t* p_filter);

	void FixParameters(
		enum AVPixelFormat* pi_fmt,
		bool* pb_has_a, fourcc_t fmt);
	[[nodiscard]] int GetParameters(ScalerConfiguration* p_cfg,
		const video_format_t* p_fmti,
		const video_format_t* p_fmto,
		int i_sws_flags_default);

	[[nodiscard]] int Init(filter_t* p_filter);
} // namespace FFmpeg