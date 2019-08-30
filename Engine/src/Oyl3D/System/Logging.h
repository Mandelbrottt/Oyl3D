#pragma once

#include "Oyl3D/Common.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace oyl {

//TODO: Add file logging

class Log {
public:
	static void init();

	inline static Ref<spdlog::logger>& getEngineLogger() { return s_engineLogger; }
	inline static Ref<spdlog::logger>& getGameLogger() { return s_gameLogger; }
private:
	static Ref<spdlog::logger> s_engineLogger;
	static Ref<spdlog::logger> s_gameLogger;
};

}

#if defined(OYL_LOG_CONSOLE)
#	if defined(OYL_ENGINE)
#		define OYL_LOG_TRACE(...)	::oyl::Log::getEngineLogger()->trace(__VA_ARGS__)
#		define OYL_LOG_INFO(...)	::oyl::Log::getEngineLogger()->info(__VA_ARGS__)
#		define OYL_LOG_WARN(...)	::oyl::Log::getEngineLogger()->warn(__VA_ARGS__)
#		define OYL_LOG_ERROR(...)	::oyl::Log::getEngineLogger()->error(__VA_ARGS__)
#		define OYL_LOG_FATAL(...)//	::oyl::Log::getEngineLogger()->fatal(__VA_ARGS__)
#	else						
#		define OYL_LOG_TRACE(...)	::oyl::Log::getGameLogger()->trace(__VA_ARGS__)
#		define OYL_LOG_INFO(...)	::oyl::Log::getGameLogger()->info(__VA_ARGS__)
#		define OYL_LOG_WARN(...)	::oyl::Log::getGameLogger()->warn(__VA_ARGS__)
#		define OYL_LOG_ERROR(...)	::oyl::Log::getGameLogger()->error(__VA_ARGS__)
#		define OYL_LOG_FATAL(...)//	::oyl::Log::getGameLogger()->fatal(__VA_ARGS__)
#	endif
#else
#	define OYL_LOG_TRACE(...)
#	define OYL_LOG_INFO(...)
#	define OYL_LOG_WARN(...)
#	define OYL_LOG_ERROR(...)
#	define OYL_LOG_FATAL(...)
#endif

#define OYL_LOG(...) OYL_LOG_TRACE(__VA_ARGS__)