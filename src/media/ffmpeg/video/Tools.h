#pragma once 

#include <cstdlib>
#include <cstdint>

#include <limits.h>
#include "Types.h"
#include "Config.h"

#include <search.h>
#include <winsock.h>

#include "Threads.h"
#include "TFunctions.h"

#include <stdatomic.h>

#define container_of(ptr, type, member) \
    ((type *)(((char *)(ptr)) - offsetof(type, member)))

namespace FFmpeg {
 

} // namespace FFmpeg
