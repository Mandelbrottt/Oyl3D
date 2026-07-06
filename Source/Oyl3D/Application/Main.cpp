#include "Main.h"

#include <cstdlib>
#include <filesystem>
#include <Windows.h>

#include <Core/Application/ApplicationEvents.h>
#include <Core/Application/CommandLine.h>
#include <Core/Logging/Logging.h>
#include <Core/Modules/MainWindowModule.h>
#include <Core/Modules/ModuleRegistry.h>
#include <Core/Profiling/Profiler.h>
#include <Core/Time/Time.h>

static bool g_running = true;

int
Main(int a_argc, char** a_argv);

#if defined(OYL_WINDOWS)
int
WINAPI
WinMain(
	_In_ HINSTANCE /* hInstance */,
	_In_opt_ HINSTANCE /* hPrevInstance */,
	_In_ LPSTR /* lpCmdLine */,
	_In_ int /* nShowCmd */

)
{
	return Main(__argc, __argv);
}
#else
int
main(int a_argc, char** a_argv)
{
	return Main(a_argc, a_argv);
}
#endif

int
Main(int a_argc, char** a_argv)
{
	Oyl::CommandLine::Init(a_argc - 1, a_argv + 1);

	Oyl::ApplicationInit();

	while (g_running)
	{
		Oyl::ApplicationUpdate();
		OYL_FRAME_MARK();
	}

	Oyl::ApplicationShutdown();

	Oyl::CommandLine::Shutdown();

	return 0;
}

namespace Oyl
{
	struct CoreApplicationData
	{
		bool shouldGameUpdate;

		uint64 maxFrameRate;

		ModuleRegistry moduleRegistry;

		Delegate<void()> quitDelegate;
	};

	static CoreApplicationData g_data = {
		.shouldGameUpdate = true,
		.maxFrameRate = 0,
		.moduleRegistry = {}
	};

	class ApplicationCloseRequestEventListener : public Module
	{
		OYL_DECLARE_MODULE(ApplicationCloseRequestEventListener);

	public:
		void
		OnInit() override
		{
			RegisterEventListener(&ApplicationCloseRequestEventListener::OnApplicationCloseRequestEvent);
		}

		void
		OnApplicationCloseRequestEvent(const ApplicationCloseRequestEvent&) const
		{
			g_running = false;
		}
	};

	void
	ApplicationInit()
	{
		SetupConsole();

#ifdef OYL_PROFILE
		// Allow users to dynamically set whether they want to profile over the network or not
		// By default, only work over localhost
		if (!Oyl::CommandLine::IsPresent("profile_network"))
		{
			SetEnvironmentVariableA("TRACY_ONLY_LOCALHOST", "1");
		}
#endif

		OYL_PROFILER_INIT();
		OYL_PROFILE_FUNCTION();

		Time::Detail::Init();
		Logging::Detail::Init();

		auto& registry = g_data.moduleRegistry;

		registry.RegisterModule<ApplicationCloseRequestEventListener>();
		registry.RegisterModule<MainWindowModule>();
	}

	void
	ApplicationUpdate()
	{
		OYL_PROFILE_FUNCTION();

		Time::Detail::Update();

		if (g_data.shouldGameUpdate)
		{
			OYL_PROFILE_SCOPE("Module Updates");
			for (Module* module : g_data.moduleRegistry)
			{
				module->OnUpdate();
			}
		}

		Logging::Detail::Flush();

		static auto lastTick = Time::Detail::ImmediateElapsedTicks();

		// If g_data.maxFrameRate is 0, uncap framerate
		uint64 minTicksPerFrame = g_data.maxFrameRate
			                          ? (uint64) (Time::Detail::TickResolution() * (1.0 / g_data.maxFrameRate))
			                          : 0;

		auto minNextFrameTicks = lastTick + minTicksPerFrame;

		uint64 thisTick;
		while ((thisTick = Time::Detail::ImmediateElapsedTicks()) < minNextFrameTicks) {}

		lastTick = thisTick;
	}

	void
	ApplicationShutdown()
	{
		OYL_PROFILE_FUNCTION();

		OYL_LOG("Shutting Down");
		Logging::Detail::Shutdown();
	}

	void
	SetupConsole()
	{
		// Attach to the console if launched from the command line, otherwise create one
		if (!AttachConsole(ATTACH_PARENT_PROCESS))
			AllocConsole();

		FILE* inFileStream;
		FILE* outFileStream;
		FILE* errFileStream;

		// Direct input and output to the console
		freopen_s(&inFileStream, "CONIN$", "r", stdin);
		freopen_s(&outFileStream, "CONOUT$", "w", stdout);
		freopen_s(&errFileStream, "CONOUT$", "w", stderr);
	}

	void
	ShutdownConsole()
	{
		FreeConsole();
	}
}
