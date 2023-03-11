module;

#include "Common.h"

export module Core;

export import Core.Main;

import Glfw;
import Yaml;

import <iostream>;

export namespace Rearm
{
	REARM_CORE_EXPORT
	void
	Init()
	{
		printf("Something\n");

		auto result = glfwInit();
		printf("%d\n", result);

		YAML::Emitter emitter;
		emitter << "YAML";
		printf("%s\n", emitter.c_str());
	}
	
	REARM_CORE_EXPORT
	void
	Shutdown()
	{
		printf("Something Else\n");
	}
}