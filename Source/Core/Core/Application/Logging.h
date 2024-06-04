#pragma once

#include "Core/Common.h"

#include <spdlog/logger.h>

namespace Rearm::Logging
{
	R_CORE_EXPORT
	void
	Init();

	R_CORE_EXPORT
	void
	Shutdown();

	R_CORE_EXPORT
	spdlog::logger& 
	GetCoreLogger();
}
