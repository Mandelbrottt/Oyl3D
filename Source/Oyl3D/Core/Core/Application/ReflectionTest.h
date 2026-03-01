#pragma once

#include "Module.h"

//#include "../.Generated/Headers/ReflectionTest.gen.h"

namespace Test
{
	class Something : public Oyl::Module
	{
		//GeneratedBody();

	public:
		Something();

		virtual
		~Something();

		void
		OnUpdate() override;

		void
		DoSomething();

		int something = 5;

		int member = 0;

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
