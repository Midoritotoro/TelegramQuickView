#pragma once 

#include "Types.h"

namespace FFmpeg {
    [[nodiscard]] int GetSwsColorspace(const video_format_t* format);
    void SetColorspace(filter_sys_t* p_sys);
} // namespace FFmpeg