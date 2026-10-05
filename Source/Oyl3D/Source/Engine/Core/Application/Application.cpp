#include "Application.h"

#include <atomic>
#include <cstdlib>

namespace Oyl::Application
{
	static bool g_exitRequested = false;
	static int g_exitCode = 0;
	static uint32 g_targetFrameRate = 0;
	static std::atomic<uint32> g_targetFramerateHint = 0;

	void
	RequestExit(int a_exitCode)
	{
		g_exitCode = a_exitCode;
		g_exitRequested = true;
	}

	void
	ForceExit(int a_exitCode)
	{
		RequestExit(a_exitCode);
		exit(a_exitCode);
	}

	bool
	IsExitRequested(int* a_outExitCode)
	{
		if (a_outExitCode)
			*a_outExitCode = g_exitCode;
		return g_exitRequested;
	}

	uint32
	TargetFramerate()
	{
		return g_targetFrameRate;
	}

	void
	SetTargetFramerate(uint32 a_target)
	{
		g_targetFramerateHint = a_target;
	}

	void
	Internal::UpdateTargetFramerate()
	{
		g_targetFrameRate = g_targetFramerateHint;
	}
}
