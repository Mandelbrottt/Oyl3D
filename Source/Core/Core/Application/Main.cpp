#include "pch.h"
#include "Main.h"

namespace Rearm
{
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

	void
	Shutdown()
	{
		printf("Something Else\n");
	}
}
