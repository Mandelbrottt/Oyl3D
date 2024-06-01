#include "pch.h"
#include "Main.h"
#include "Logging.h"

namespace Rearm
{
	static OnShouldGameUpdateFn g_onShouldGameUpdateCallback;

	void
	Init(
		const CoreInitParameters& a_params
	)
	{
		g_onShouldGameUpdateCallback = a_params.onShouldGameUpdateCallback;
		
		Logging::Init();
		Logging::GetCoreLogger().info("Initializing...");
	}

	void
	Update()
	{
		bool shouldGameUpdate = true;
		if (g_onShouldGameUpdateCallback)
		{
			shouldGameUpdate = g_onShouldGameUpdateCallback();
		}

		R_UNUSED(shouldGameUpdate);
	}

	void
	Shutdown()
	{
		Logging::GetCoreLogger().info("Shutting Down");
	}
}
