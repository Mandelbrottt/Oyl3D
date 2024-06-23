#include "pch.h"

#include "Logging.h"

#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/ringbuffer_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Oyl::Logging
{
	// Default initialize the core logger to a ring buffer logger
	// For use by anything that, for whatever reason, must be run before logging is initialized
	// Once logging is initialized, we dump all messages from the pre-init logger to the actual logger
	static std::shared_ptr<spdlog::logger> g_coreLogger = 
		spdlog::create<spdlog::sinks::ringbuffer_sink_st>("PRE-INIT", 32);

	// TODO: Add multiple loggers at once https://github.com/gabime/spdlog/wiki/1.-QuickStart#create-a-logger-with-multiple-sinks-each-sink-with-its-own-formatting-and-log-level
	namespace Detail
	{
		void
		Init()
		{
			OYL_PROFILE_FUNCTION();

			// Cache previous logger
			auto preInitLogger = g_coreLogger;

			// Initialize the main logger
			auto formatter = std::make_unique<spdlog::pattern_formatter>();
			formatter->set_pattern("%^[%T][%l][%n] %v%$");
			g_coreLogger = spdlog::stdout_color_mt("CORE");
			g_coreLogger->set_level(spdlog::level::debug);
			g_coreLogger->set_formatter(std::move(formatter));

			// Log the pre-init messages to the core logger
			auto preInitLogSink = 
				std::static_pointer_cast<spdlog::sinks::ringbuffer_sink_st>(preInitLogger->sinks().front());
			auto preInitLogs = preInitLogSink->last_raw();
			for (auto& msg : preInitLogs)
			{
				g_coreLogger->log(msg.time, msg.source, msg.level, msg.payload);
			}
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
