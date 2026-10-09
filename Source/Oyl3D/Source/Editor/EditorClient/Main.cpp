#include <iostream>

#include <Core/Application/Application.h>
#include <Core/CommandLine/CommandLine.h>
#include <Core/Logging/Logging.h>
#include <Core/Modules/ModuleManager.h>
#include <Core/Time/Time.h>

#ifndef OYL_EXPOSE_PROFILER_CONTROLS
#	define OYL_EXPOSE_PROFILER_CONTROLS
#endif
#include <Core/Profiling/Profiling.h>
#include <Core/Time/IFrameTimer.h>

#include "MainLoop.h"

static Oyl::IFrameTimer* g_frameTimer = nullptr;

static
void
WaitUntilNextFrame();

int
main(int a_argc, char** a_argv)
{
	OYL_PROFILER_INIT();
	OYL_PROFILE_FUNCTION();

	Oyl::ModuleManager::LoadModulePointer("Core");
	Oyl::Time::Internal::Init(g_frameTimer);

	Oyl::ModuleManager::Get().LoadModule("SpdLogLogger");

	Oyl::CommandLine::Init(a_argc, a_argv);

	// Scope the loop to manage when the MainLoop destructor is called
	{
		Oyl::EditorClientMainLoop loop;
		while (!Oyl::Application::IsExitRequested())
		{
			Oyl::Time::Internal::Update();

			loop.Update();

			Oyl::Internal::GetCurrentLogger()->Flush();

			// Once we are done all the processing for this frame, we can wait for the next one
			WaitUntilNextFrame();
		}
	}

	Oyl::CommandLine::Shutdown();

	Oyl::ModuleManager::Get().UnloadModule("SpdLogLogger");
	Oyl::ModuleManager::Get().UnloadModule("Core");

	OYL_PROFILER_SHUTDOWN();
}

void
WaitUntilNextFrame()
{
	// Intentionally use g_frameTimer instead of Oyl::Time::GetFrameTimer since that is user overrideable
	static auto lastTick = g_frameTimer->ElapsedProcessorTicks();

	Oyl::Application::Internal::UpdateTargetFramerate();
	// If target framerate is 0, don't cap framerate
	uint32 targetFrameRate = Oyl::Application::TargetFramerate();
	uint64 minNumTicksPerFrame = 0;
	if (targetFrameRate > 0)
		minNumTicksPerFrame = (uint64) (g_frameTimer->TickResolution() * (1.0 / targetFrameRate));

	auto targetTickForNextFrame = lastTick + minNumTicksPerFrame;

	// Wait for next frame
	uint64 thisTick;
	do
	{
		thisTick = g_frameTimer->ElapsedProcessorTicks();
	} while (thisTick < targetTickForNextFrame);

	lastTick = thisTick;
}

#if defined(_WIN32)
#include <Windows.h>

#include <Core/Time/Windows/WindowsFrameTimer.h>

int
WINAPI
WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	if (!AttachConsole(ATTACH_PARENT_PROCESS))
		AllocConsole();

	FILE* inFileStream;
	FILE* outFileStream;
	FILE* errFileStream;

	// Direct input and output to the console
	freopen_s(&inFileStream, "CONIN$", "r", stdin);
	freopen_s(&outFileStream, "CONOUT$", "w", stdout);
	freopen_s(&errFileStream, "CONOUT$", "w", stderr);

#if defined(OYL_PROFILE)
	{
		// Allow users to dynamically set whether they want to profile over the network or not
		// By default, only work over localhost
		bool doProfileNetwork = false;
		for (int i = 0; i < __argc; i++)
		{
			if (strcmp(__argv[i], "profile_network"))
			{
				doProfileNetwork = true;
				break;
			}
		}
		if (doProfileNetwork)
		{
		#if defined(OYL_WINDOWS)
			SetEnvironmentVariableA("TRACY_ONLY_LOCALHOST", "1");
		#endif
		}
	}
#endif

	g_frameTimer = new Oyl::WindowsFrameTimer;
	int result = main(__argc, __argv);

	fclose(inFileStream);
	fclose(outFileStream);
	fclose(errFileStream);

	FreeConsole();

	return result;
}

#endif
