#include "pch.h"
#include "Main2.h"

namespace Rearm
{
	void
	SomethingElse();

	REARM_CORE_EXPORT
	void
	Something()
	{
		printf("Something Else\n");
	}
}

void
Rearm::SomethingElse()
{
	printf("Something hahahee\n");
}
