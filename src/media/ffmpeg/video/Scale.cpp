#include "Scale.h"

#include <cassert>

#ifdef Q_OS_WIN
	#include <Windows.h>
#endif // Q_OS_WIN

namespace FFmpeg {

	int GetCpuCount() {
#if defined Q_OS_WIN
		SYSTEM_INFO info;
		GetSystemInfo(&info);

		return info.dwNumberOfProcessors;
#endif // Q_OS_WIN
		return 1;
	}

}