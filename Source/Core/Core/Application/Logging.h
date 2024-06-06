#pragma once

#include "Core/Common.h"

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
