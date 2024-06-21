#include "pch.h"

#include "Logging.h"

#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Oyl::Logging
{
	static std::shared_ptr<spdlog::logger> g_coreLogger;

	// TODO: Add multiple loggers at once https://github.com/gabime/spdlog/wiki/1.-QuickStart#create-a-logger-with-multiple-sinks-each-sink-with-its-own-formatting-and-log-level
	namespace Detail
	{
		void
		Init()
		{
			OYL_PROFILE_FUNCTION();
			
			auto formatter = std::make_unique<spdlog::pattern_formatter>();
			formatter->set_pattern("%^[%T][%l][%n] %v%$");

			g_coreLogger = spdlog::stdout_color_mt("CORE");
			g_coreLogger->set_level(spdlog::level::debug);
			g_coreLogger->set_formatter(std::move(formatter));
		}

		void
		Shutdown()
		{
			OYL_PROFILE_FUNCTION();
			
			g_coreLogger.reset();
		}
	}

	spdlog::logger&
	GetLogger() { return *g_coreLogger; }
}
