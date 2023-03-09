module;
#include "Common.h"

export module Core;

export import Core.Main;

import <iostream>;

export namespace Rearm
{
	REARM_CORE_EXPORT
	void
	Init()
	{
		printf("Something\n");
	}
	
	REARM_CORE_EXPORT
	void
	Shutdown()
	{
		printf("Something Else\n");
	}
}