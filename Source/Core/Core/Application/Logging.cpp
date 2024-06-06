#include "pch.h"

// Define SPDLOG_LEVEL_NAMES before including Logging
#define SPDLOG_LEVEL_NAMES \
	{ \
		spdlog::string_view_t("TRACE", 5), \
		spdlog::string_view_t("DEBUG", 5), \
		spdlog::string_view_t("INFO", 4), \
		spdlog::string_view_t("WARNING", 7), \
		spdlog::string_view_t("ERROR", 5), \
		spdlog::string_view_t("CRITICAL", 8), \
		spdlog::string_view_t("OFF", 3), \
	}

#include "Logging.h"

#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Oyl::Logging
{
	static std::shared_ptr<spdlog::logger> g_coreLogger;

	// TODO: Add multiple loggers at once https://github.com/gabime/spdlog/wiki/1.-QuickStart#create-a-logger-with-multiple-sinks-each-sink-with-its-own-formatting-and-log-level

	void
	Init()
	{
		auto formatter = std::make_unique<spdlog::pattern_formatter>();
		formatter->set_pattern("%^[%T][%l][%n] %v%$");

		g_coreLogger = spdlog::stdout_color_mt("CORE");
		g_coreLogger->set_level(spdlog::level::debug);
		g_coreLogger->set_formatter(std::move(formatter));
	}

	void
	Shutdown() { }

	spdlog::logger&
	GetCoreLogger() { return *g_coreLogger; }
}
