#include "pch.h"
#include "Main.h"
#include "Logging.h"

#include <iostream>

namespace Oyl
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
		OYL_LOG("Initializing...");
	}

	void
	Update()
	{
		if (g_shouldGameUpdate)
		{
			OYL_LOG("game update");
		}

		OYL_LOG("regular update");

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
		OYL_LOG("Shutting Down");
		Logging::Shutdown();
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
