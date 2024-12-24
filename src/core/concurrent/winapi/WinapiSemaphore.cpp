#include "WinapiSemaphore.h"

#ifdef USE_WINAPI

namespace concurrent {

auto semaphore::implementation::create() -> pointer {
	auto result = CreateSemaphore(nullptr, 0, 1, nullptr);
	if (!result) {
		std::terminate();
	}
	return result;
}

void semaphore::implementation::operator()(pointer value) {
	if (value) {
		CloseHandle(value);
	}
};

void semaphore::acquire() {
	WaitForSingleObject(_handle.get(), INFINITE);
}

void semaphore::release() {
	ReleaseSemaphore(_handle.get(), 1, nullptr);
}

} // namespace concurrent

#endif // USE_WINAPI
