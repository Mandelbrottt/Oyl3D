module;
#include "pch.h"
#include "Common.h"

export module Core.Main;

namespace Rearm
{
	REARM_CORE_EXPORT
	void
	SomethingElse();

	REARM_CORE_EXPORT
	void
	Something()
	{
		printf("Something Else\n");
	}
}

module :private;

void
Rearm::SomethingElse()
{
	printf("Something hahahee\n");
}
