#pragma once

#include "../../../core/Types.h"
#include "../../../core/Time.h"

#include <stdatomic.h>
#include <Windows.h>

#include "VideoHelper.h"

#ifdef __STDC_NO_ATOMICS__ 
#undef __STDC_NO_ATOMICS__ 
#endif // __STDC_NO_ATOMICS__ 


extern "C" {
    #include <libswscale/swscale.h>
}


namespace FFmpeg {

    struct module_t;
    struct plagin_t;
    struct video_format_t;
    using fourcc_t = uint32_t;



} // namespace FFmpeg