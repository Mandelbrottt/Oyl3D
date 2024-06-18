#include "pch.h"
#include "Main.h"

#include <iostream>

#include "Core/Logging/Logging.h"
#include "Core/Time/Time.h"

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

		Time::Detail::Init();

		Logging::Init();
		OYL_LOG("Initializing...");
	}

	void
	Update()
	{
		Time::Detail::Update();
		
		if (g_shouldGameUpdate)
		{
			OYL_LOG("game update {}", Time::DeltaTime());
		}

		OYL_LOG("regular update {}", Time::ElapsedTime());

		auto elapsed1 = Time::Detail::ImmediateElapsedTime();
		auto elapsed2 = Time::Detail::ImmediateElapsedTime();

		OYL_LOG("1: {}\t2: {}", elapsed1, elapsed2);

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
