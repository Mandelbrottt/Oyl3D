#include "oylpch.h"
#include "Win32Platform.h"

namespace oyl {

Platform* Platform::s_instance = new Win32Platform;

float Win32Platform::getTimeImpl() {
	LARGE_INTEGER li;
	static uint64_t timeStart = [&]()->float {
		QueryPerformanceCounter(&li);
		return li.QuadPart;
	}();
	static double frequency = [&]()->float {
		QueryPerformanceFrequency(&li);
		return double(li.QuadPart);
	}();

	QueryPerformanceCounter(&li);
	return double(li.QuadPart - timeStart) / frequency;
}

}


