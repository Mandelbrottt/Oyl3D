#pragma once

#include "Core/Common.h"

#include <spdlog/logger.h>

namespace Oyl::Logging
{
	OYL_CORE_EXPORT
	void
	Init();

	OYL_CORE_EXPORT
	void
	Shutdown();

	OYL_CORE_EXPORT
	spdlog::logger& 
	GetCoreLogger();
}
