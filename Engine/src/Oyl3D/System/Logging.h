#pragma once

#include "Oyl3D/Common.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace oyl {

//TODO: Add file logging

class Log {
public:
	static void init();

	inline static std::shared_ptr<spdlog::logger>& getEngineLogger() { return s_engineLogger; }
	inline static std::shared_ptr<spdlog::logger>& getGameLogger() { return s_gameLogger; }
private:
	static std::shared_ptr<spdlog::logger> s_engineLogger;
	static std::shared_ptr<spdlog::logger> s_gameLogger;
};

}

#if defined(OYL_DEBUG)
#	if defined(OYL_ENGINE)
#		define LOG_TRACE(...)	::oyl::Log::getEngineLogger()->trace(__VA_ARGS__)
#		define LOG_INFO(...)	::oyl::Log::getEngineLogger()->info(__VA_ARGS__)
#		define LOG_WARN(...)	::oyl::Log::getEngineLogger()->warn(__VA_ARGS__)
#		define LOG_ERROR(...)	::oyl::Log::getEngineLogger()->error(__VA_ARGS__)
#		define LOG_FATAL(...)//	::oyl::Log::getEngineLogger()->fatal(__VA_ARGS__)
#	else						
#		define LOG_TRACE(...)	::oyl::Log::getGameLogger()->trace(__VA_ARGS__)
#		define LOG_INFO(...)	::oyl::Log::getGameLogger()->info(__VA_ARGS__)
#		define LOG_WARN(...)	::oyl::Log::getGameLogger()->warn(__VA_ARGS__)
#		define LOG_ERROR(...)	::oyl::Log::getGameLogger()->error(__VA_ARGS__)
#		define LOG_FATAL(...)//	::oyl::Log::getGameLogger()->fatal(__VA_ARGS__)
#	endif
#else
#	define LOG_TRACE(...)
#	define LOG_INFO(...)
#	define LOG_WARN(...)
#	define LOG_ERROR(...)
#	define LOG_FATAL(...)
#endif

#define LOG(...) LOG_TRACE(__VA_ARGS__)