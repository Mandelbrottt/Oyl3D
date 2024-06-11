#pragma once

// Define SPDLOG_LEVEL_NAMES before including spdlog
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

#define SPDLOG_COMPILED_LIB
#if defined(OYL_BUILD_SHARED_LIB)
	#define SPDLOG_SHARED_LIB
#endif

#include <spdlog/logger.h>
#include <spdlog/spdlog.h>