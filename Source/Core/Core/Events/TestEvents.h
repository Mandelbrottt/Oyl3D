#pragma once

#include "Event.h"

namespace Oyl
{
	struct TestEvent1 : Event
	{
		OYL_DECLARE_EVENT(TestEvent1);

		int a;
	};
	
	struct TestEvent2 : Event
	{
		OYL_DECLARE_EVENT(TestEvent2);

		int b;
	};
}