#pragma once

#define SPDLOG_COMPILED_LIB
#if defined(OYL_BUILD_SHARED_LIB)
	#define SPDLOG_SHARED_LIB
#endif

#include <spdlog/logger.h>
#include <spdlog/spdlog.h>