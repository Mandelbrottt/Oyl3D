#pragma once

#include "Application/Common.h"

namespace Oyl
{
	OYL_APPLICATION_API
	extern
	void
	ApplicationInit();

	OYL_APPLICATION_API
	extern
	void
	ApplicationUpdate();

	OYL_APPLICATION_API
	extern
	void
	ApplicationShutdown();

	OYL_APPLICATION_API
	extern
	void
	SetupConsole();

	OYL_APPLICATION_API
	extern
	void
	ShutdownConsole();
}
