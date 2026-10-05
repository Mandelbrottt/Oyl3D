#pragma once

#include "Core/PPExport.h"
#include "Core/Types/PrimitiveTypes.h"

namespace Oyl::Application
{
	CORE_EXPORT
	void
	RequestExit(int a_exitCode = 0);

	CORE_EXPORT
	void
	ForceExit(int a_exitCode = 0);

	CORE_EXPORT
	bool
	IsExitRequested(int* a_outExitCode = nullptr);

	CORE_EXPORT
	uint32
	TargetFramerate();

	CORE_EXPORT
	void
	SetTargetFramerate(uint32 a_target);

	namespace Internal
	{
		CORE_EXPORT
		void
		UpdateTargetFramerate();
	}
}
