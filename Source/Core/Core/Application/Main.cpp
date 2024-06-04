#include "pch.h"
#include "Main.h"
#include "Logging.h"

#include <iostream>

namespace Rearm
{
	static CoreInitParameters g_params;

	bool g_shouldGameUpdate = true;

	void
	Init(
		const CoreInitParameters& a_params
	)
	{
		g_params = a_params;

		Logging::Init();
		Logging::GetCoreLogger().info("Initializing...");
	}

	void
	Update()
	{
		if (g_shouldGameUpdate)
		{
			Logging::GetCoreLogger().info("game update");
		}

		Logging::GetCoreLogger().info("regular update");

		char in = std::cin.get();
		if (in == 'q')
		{
			g_params.onApplicationShouldQuitCallback();
		}
		if (in == 'g')
		{
			g_shouldGameUpdate = !g_shouldGameUpdate;
		}
	}

	void
	Shutdown()
	{
		Logging::GetCoreLogger().info("Shutting Down");
	}

	bool
	GetShouldGameUpdate() noexcept
	{
		return g_shouldGameUpdate;
	}

	void
	SetShouldGameUpdate(
		bool a_value
	) noexcept
	{
		g_shouldGameUpdate = a_value;
	}
}
