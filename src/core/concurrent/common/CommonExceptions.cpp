#include "../../Time.h"
#include "CommonConfig.h"

#ifndef USE_WINAPI_TIME

#include <float.h>

namespace concurrent {

void toggle_fp_exceptions(bool throwing) {

}

} // namespace concurrent

#endif // !USE_WINAPI_TIME
