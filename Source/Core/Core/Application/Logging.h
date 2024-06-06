#pragma once

#include "Core/Common.h"

#define SPDLOG_COMPILED_LIB
#define SPDLOG_SHARED_LIB
#include <spdlog/logger.h>

namespace Oyl::Logging
{
	OYL_CORE_API
	void
	Init();

	OYL_CORE_API
	void
	Shutdown();

	spdlog::logger& 
	GetCoreLogger();
	OYL_CORE_API
}
}
