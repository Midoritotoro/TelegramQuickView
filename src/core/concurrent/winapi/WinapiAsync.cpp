#include "WinapiAsync.h"

#ifdef USE_WINAPI

#include <concrt.h>

namespace concurrent::details {

void async_plain(void (*callable)(void*), void *argument) {
	printf("async_plain called\n");
	Concurrency::CurrentScheduler::ScheduleTask(callable, argument);
}

} // namespace concurrent::details

#endif // USE_WINAPI
