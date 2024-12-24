#pragma once

#include "CommonConfig.h"

#include <semaphore>

namespace concurrent {

class semaphore {
public:
	semaphore() = default;
	semaphore(const semaphore &other) = delete;
	semaphore &operator=(const semaphore &other) = delete;
	semaphore(semaphore &&other) = delete;
	semaphore &operator=(semaphore &&other) = delete;

	void acquire() {
		_impl.acquire();
	}
	void release() {
		_impl.release();
	}

private:
	std::binary_semaphore _impl{0};

};

} // namespace concurrent
