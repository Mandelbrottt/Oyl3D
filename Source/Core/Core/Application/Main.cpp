#include "pch.h"
#include "Main.h"
#include "Core/Logging/Logging.h"

#include <iostream>

#include "Core/Math/Matrix4.h"

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

		OYL_LOG_DEBUG("Testing");
		OYL_LOG_WARNING("Testing");
		OYL_LOG_ERROR("Testing");
		OYL_LOG_FATAL("Testing");

		auto start = std::chrono::system_clock::now();
		volatile float a = 0;
		for (int i = 0; i < 10000000; i++)
		{
			Matrix4 matA = Matrix4::Identity((float) i), matB = Matrix4::Identity((float) i + 1);
			matA = matA * matB;
			a = matA.data[0];
		}
		auto end = std::chrono::system_clock::now();

		OYL_LOG("Elapsed Time: {}ms", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
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
