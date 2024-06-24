#include "pch.h"
#include "Main.h"

#include <iostream>

#include "CommandLine.h"
#include "ModuleRegistry.h"

#include "Core/Logging/Logging.h"
#include "Core/Profiling/Profiler.h"
#include "Core/Time/Time.h"

namespace Oyl::Detail
{
	struct CoreApplicationData
	{
		CoreInitParameters params;

		bool shouldGameUpdate;

		ModuleRegistry moduleRegistry;
	};

	static CoreApplicationData g_data;
	
	void
	Init(const CoreInitParameters& a_params)
	{
		g_data.params = a_params;
		g_data.shouldGameUpdate = true;

		// Init Time before profiling, as profiling relies on Time
		Time::Detail::Init();
		OYL_PROFILE_FUNCTION();
		
		Logging::Detail::Init();
	}

	void
	Update()
	{
		OYL_PROFILE_FUNCTION();
		
		Time::Detail::Update();

		// TODO: Implement core and game modules
		//if (g_data.shouldGameUpdate)
		//{
		//	OYL_LOG("game update {}", Time::DeltaTime());
		//}

		for (Module* module : g_data.moduleRegistry)
		{
			module->OnUpdate();
		}
		
		char in = std::cin.get();
		if (in == 'q')
		{
			g_data.params.onApplicationShouldQuitCallback();
		}
		if (in == 'g')
		{
			g_data.shouldGameUpdate = !g_data.shouldGameUpdate;
		}
	}

	void
	Shutdown()
	{
		OYL_PROFILE_FUNCTION();
		
		OYL_LOG("Shutting Down");
		Logging::Detail::Shutdown();
	}

	bool
	GetShouldGameUpdate() noexcept
	{
		return g_data.shouldGameUpdate;
	}

	void
	SetShouldGameUpdate(
		bool a_value
	) noexcept
	{
		g_data.shouldGameUpdate = a_value;
	}
}
