#include "oylpch.h"
#include "Logging.h"

namespace oyl
{
    Ref<spdlog::logger> oyl::Log::s_engineLogger = nullptr;
    Ref<spdlog::logger> oyl::Log::s_gameLogger   = nullptr;

    void Log::init()
    {
        spdlog::set_pattern("%^[%T] %n: %v%$");

        s_engineLogger = spdlog::stdout_color_mt("Engine");
        s_engineLogger->set_level(spdlog::level::trace);

        s_gameLogger = spdlog::stdout_color_mt("Game");
        s_gameLogger->set_level(spdlog::level::trace);
    }
}
