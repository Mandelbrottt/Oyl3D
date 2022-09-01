#include "pch.h"

#include "Main.h"

#include <iostream>

#include <Windows.h>
#include <Psapi.h>

#include <type_traits>
#include <typeinfo>

#include <chrono>

#include "Reflection/Reflection.h"

class SomeClass
{
	ReflectDeclare(SomeClass)
	{
		ReflectField(a);
		ReflectField(b);
		ReflectFunction(SumWithOther);
	}

public:
	virtual ~SomeClass() = default;

	int a = 0;

	int b = 0;

	int SumWithOther(const SomeClass& a_other) const
	{
		return a + b + a_other.a + a_other.b;
	}
};

namespace Rearm
{
	void
	Init()
	{
		HMODULE hModule = GetModuleHandle(TEXT("RearmCore.dll"));
		
		struct MemberFunctionPointer
		{
			void* fp;
			size_t offset;
		};

		MemberFunctionPointer mfp;
		auto sumWithOther = &SomeClass::SumWithOther;
		std::memcpy(&mfp, &sumWithOther, sizeof(MemberFunctionPointer));

		uintptr_t mfpAdjusted = reinterpret_cast<uintptr_t>(mfp.fp) - reinterpret_cast<uintptr_t>(hModule);

		printf("Base (%p), Mfp (%p), Diff (%p)\n", hModule, mfp.fp, reinterpret_cast<void*>(mfpAdjusted));
		
		SomeClass sc;
		sc.a = 5;
		sc.b = 10;

		auto& someClassType = Rearm::Reflection::Type::Register<SomeClass>();

		auto* sumWithOtherFn = someClassType.GetFunction("SumWithOther");

		SomeClass other;
		other.a = 50;
		other.b = 100;

		volatile int numIterations = 1000000;

		namespace ch = std::chrono;

		auto start = ch::high_resolution_clock::now();

		for (int i = 0; i < numIterations; i++)
		{
			volatile int _ = sc.SumWithOther(other);
		}

		auto stop = ch::high_resolution_clock::now();

		auto firstDuration = ch::duration_cast<ch::microseconds>(stop - start).count();
		printf("Static: %llu microseconds\n", firstDuration);

		start = ch::high_resolution_clock::now();
		
		for (int i = 0; i < numIterations; i++)
		{
			volatile int _ = sumWithOtherFn->Call<int(const SomeClass&)>(&sc, other);
		}
		
		stop = ch::high_resolution_clock::now();

		auto secondDuration = ch::duration_cast<ch::microseconds>(stop - start).count();
		printf("Reflection: %llu microseconds\n", secondDuration);

		printf("Ratio of Reflection to Static: %f\n", (double) secondDuration / firstDuration);
	}

	void
	Shutdown() { }
}
