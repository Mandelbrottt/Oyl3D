#include "pch.h"
#include "Main.h"

#include <iostream>

#include "CommandLine.h"
#include "Module.h"
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

		uint64 maxFrameRate;

		ModuleRegistry moduleRegistry;
	};

	static CoreApplicationData g_data;

	void
	Init(const CoreInitParameters& a_params)
	{
		OYL_PROFILE_FUNCTION();

		g_data.params           = a_params;
		g_data.shouldGameUpdate = true;
		g_data.maxFrameRate     = 60;

		Time::Detail::Init();

		Logging::Detail::Init();

		auto& registry = g_data.moduleRegistry;
		registry.SetOnEventCallback(OnEvent);
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
		{
			OYL_PROFILE_SCOPE("Module Updates");
			for (Module* module : g_data.moduleRegistry)
			{
				module->OnUpdate();
			}
		}

		//char in = std::cin.get();
		//if (in == 'q')
		//{
		//	g_data.params.onApplicationShouldQuitCallback();
		//}
		//if (in == 'g')
		//{
		//	g_data.shouldGameUpdate = !g_data.shouldGameUpdate;
		//}

		static auto lastTick = Time::Detail::ImmediateElapsedTicks();
		uint64 ticksPerFrame = (uint64) (Time::Detail::TickResolution() * (1.0 / g_data.maxFrameRate));
		auto minNextFrameTicks = lastTick + ticksPerFrame;

		uint64 thisTick;
		while ((thisTick = Time::Detail::ImmediateElapsedTicks()) < minNextFrameTicks) {}
		
		lastTick = thisTick;
	}

	void OnEvent(Event& a_event)
	{
		OYL_PROFILE_FUNCTION();
		for (auto* module : g_data.moduleRegistry)
		{
			module->OnEvent(a_event);
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

	ModuleRegistry*
	GetModuleRegistry()
	{
		return &g_data.moduleRegistry;
	}
}
