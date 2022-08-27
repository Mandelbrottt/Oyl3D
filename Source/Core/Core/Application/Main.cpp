#include "pch.h"

#include "Main.h"

#include <iostream>

#include <type_traits>
#include <typeinfo>

#include "Reflection/Reflection.h"

class SomeClass
{
	ReflectDeclare(SomeClass)
	{
		ReflectField(a);
		ReflectField(b);
		ReflectFunction(Foo);
		ReflectFunction(Bar);
		ReflectFunction(Sum);
		ReflectFunction(StaticFunction, r_name = "StupidFace Carter");
		ReflectFunction(StaticSum);
	}

public:
	virtual ~SomeClass() = default;

	int a = 0;

	int b = 0;

	void
	Foo() const
	{
		printf("Foo!\n");
	}

	int
	Bar() const
	{
		return 5;
	}

	int
	Sum(int a_lhs, int a_rhs) const
	{
		return a_lhs + a_rhs;
	}

	static
	void
	StaticFunction()
	{
		printf("Static Function!\n");
	}

	static
	int
	StaticSum(const int& a_lhs, const int& a_rhs)
	{
		return a_lhs + a_rhs;
	}
};

namespace Rearm
{
	void
	Init()
	{
		SomeClass sc;

		auto& someClassType = Rearm::Reflection::Type::Register<SomeClass>();

		auto* fooFn       = someClassType.GetFunction("Foo");
		auto* barFn       = someClassType.GetFunction("Bar");
		auto* sumFn       = someClassType.GetFunction("Sum");
		auto* staticFn    = someClassType.GetFunction("StaticFunction");
		auto* staticSumFn = someClassType.GetFunction("StaticSum");

		printf("Debug Name: \"%s\", Display Name: \"%s\"\n", staticFn->Name().c_str(), staticFn->DisplayName().c_str());

		fooFn->Call(&sc);
		printf("%d\n", barFn->Call<int>(&sc));
		printf("%d\n", sumFn->Call<int>(&sc, 5, 10));

		staticFn->Call(nullptr);
		printf("%d\n", staticSumFn->Call<int>(nullptr, 5, 10));
	}

	void
	Shutdown() { }
}
