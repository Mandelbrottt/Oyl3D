module;

#include "Common.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

export module Core;

export import Core.Main;

export namespace Rearm
{
	REARM_CORE_EXPORT
	void
	Init()
	{
		printf("Something\n");

		spdlog::set_pattern("%^[%T] %n: %v%$");
		auto logger = spdlog::stdout_color_mt("Test");
		logger->error("Something {} Happened", 1);
	}
	
	REARM_CORE_EXPORT
	void
	Shutdown()
	{
		printf("Something Else\n");
	}
}