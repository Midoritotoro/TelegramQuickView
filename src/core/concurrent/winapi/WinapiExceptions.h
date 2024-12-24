#pragma once

#include "../common/CommonConfig.h"

#ifdef USE_WINAPI

#include "../common/CommonUtils.h"
#include "../common/CommonSync.h"

#include <type_traits>


namespace concurrent {

void toggle_fp_exceptions(bool throwing);

} // namespace concurrent

#endif // USE_WINAPI
