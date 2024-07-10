#pragma once

#include "ReflectionTest.generated.h"

#include "Core/Logging/Logging.h"

namespace Test
{
	class Something
	{
	public:
		Something()
		{
			OYL_LOG("Something()");
		}

		virtual
		~Something()
		{
			OYL_LOG("~Something()");
		}

		void
		DoSomething()
		{
			OYL_LOG("DoSomething()");
		}

		int something = 5;

	private:
		int m_somethingElse = 0;
	};
}

enum class SomeEnum
{
	One,
	Two,
	Three
};
