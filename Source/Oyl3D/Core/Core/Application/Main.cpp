#include "pch.h"
#include "Main.h"

#include "CommandLine.h"
#include "Core/Modules/Module.h"
#include "Core/Modules/ModuleRegistry.h"

#include "Core/Logging/Logging.h"
#include "Core/Profiling/Profiler.h"
#include "Core/Time/Time.h"

#include "SharedLibrary.h"

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

	struct TestEvent1 : Event
	{
		OYL_DECLARE_EVENT();
	};

	struct TestEvent2 : Event
	{
		OYL_DECLARE_EVENT();
	};

	class TestModule1 : public Module
	{
		OYL_DECLARE_MODULE(TestModule1);

	public:
		void OnInit() override
		{
			RegisterEventListener(&TestModule1::OnTestEvent2);

			a = 20;
		}

		void OnUpdate() override
		{
			OYL_LOG("Hello from TestModule1!");
			PostEvent(TestEvent1());
		}

		void
		OnTestEvent2(const TestEvent2&) const
		{
			OYL_LOG("TestEvent2 from TestModule1! {}", a);
		}

		int a = 5;
	};

	class TestModule2 : public Module
	{
		OYL_DECLARE_MODULE(TestModule2);

	public:
		void OnInit() override
		{
			RegisterEventListener(&TestModule2::OnTestEvent1);

			someValue = "DEADBEEF";
		}

		void OnUpdate() override
		{
			OYL_LOG("Hello from TestModule2!");
			PostEvent(TestEvent2());
		}

		void
		OnTestEvent1(const TestEvent1&) const
		{
			OYL_LOG("TestEvent1 from TestModule2! {}", someValue);
		}

		std::string someValue;
	};

	void
	Init(const CoreInitParameters& a_params)
	{
		OYL_PROFILE_FUNCTION();

		g_data.params = a_params;
		g_data.shouldGameUpdate = true;
		g_data.maxFrameRate     = 60;

		Time::Detail::Init();

		Logging::Detail::Init();

		auto& registry = g_data.moduleRegistry;
		registry.RegisterModule<TestModule1>();
		registry.RegisterModule<TestModule2>();
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

		Logging::Detail::Flush();

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