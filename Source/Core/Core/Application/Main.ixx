module;
#include "pch.h"
#include "Common.h"

export module Core;

export import Core.Main;

export namespace Rearm
{
	REARM_CORE_EXPORT
	void
	Init()
	{
		printf("Something\n");

		glfwInit();
		glfwTerminate();

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