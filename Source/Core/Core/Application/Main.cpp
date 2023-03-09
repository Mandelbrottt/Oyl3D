#include "pch.h"

#include "Main.h"

#include <iostream>

namespace Rearm
{
	void
	Init()
	{
		YAML::Emitter out;
		out << "Hello, World!";
		printf("Output YAML:\n\t%s\n", out.c_str());

		std::cout << nlohmann::json {
			{ "OneThing", 5 },
			{ "AnotherThing", true },
		}.dump(4) << "\n";
	}

	void
	Shutdown() { }
}
