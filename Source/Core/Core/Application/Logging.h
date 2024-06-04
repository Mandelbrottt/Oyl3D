#pragma once

#include "Core/Common.h"

#include <spdlog/logger.h>

namespace Rearm::Logging
{
	REARM_CORE_EXPORT
	void
	Init();

	REARM_CORE_EXPORT
	void
	Shutdown();

	REARM_CORE_EXPORT
	spdlog::logger& 
	GetCoreLogger();
}
