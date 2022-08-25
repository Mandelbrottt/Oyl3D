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
	}

public:
	virtual ~SomeClass() = default;

	int a = 0;

	int b = 0;

	void
	Foo() {}
};

namespace Rearm
{
	void
	Init()
	{
		SomeClass sc;

		auto& someClassType = Rearm::Reflection::Type::Register<SomeClass>();
		auto* aField = someClassType.GetField("a");
		auto* bField = someClassType.GetField("b");
		
		printf("%d %d\n", sc.a, sc.b);
		printf("%d %d\n", aField->GetValue<int>(sc), bField->GetValue<int>(sc));

		aField->SetValue(sc, 5);
		bField->SetValue(sc, 10);

		printf("\n");
		printf("%d %d\n", sc.a, sc.b);
		printf("%d %d\n", aField->GetValue<int>(sc), bField->GetValue<int>(sc));
	}

	void
	Shutdown() { }
}
