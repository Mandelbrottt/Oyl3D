#include <iostream>
#include <cassert>
#include <vector>

#include "Core/Interface.h"

static std::vector<std::string> g_interfaces;

// ReSharper disable once CppInconsistentNaming
extern "C" __declspec(dllexport) const std::vector<std::string>& __DllTest_Interfaces()
{
	return g_interfaces;
}

#define RegisterInterface(_class_) \
	static int __IIFE##_class_ = [] \
	{ \
		g_interfaces.push_back(#_class_); \
		return 0; \
	}(); \
	/* Trick the compiler into thinking we're using the above variable so it doesn't get optimized out */\
	__declspec(dllexport) int __UseIIFE##_class_() { return __IIFE##_class_; } \
	extern "C" __declspec(dllexport) size_t __Size##_class_() \
	{ \
		return sizeof(_class_); \
	} \
	extern "C" __declspec(dllexport) Interface* __Alloc##_class_(void* a_location, size_t a_sizeInBytes) \
	{ \
		assert(a_sizeInBytes == __Size##_class_()); \
		_class_* interfaceLocation = reinterpret_cast<_class_*>(a_location); \
		new(interfaceLocation) _class_; \
		return interfaceLocation; \
	} \
	static_assert(true, "")

class DerivedInterface : public Interface
{
public:
	DerivedInterface() = default;

	virtual ~DerivedInterface() { printf("~DerivedInterface()\n"); }

	void Foo() override
	{
		printf("Foo()\n");
	}
};

RegisterInterface(DerivedInterface);

class OtherDerivedInterface : public Interface
{
public:
	OtherDerivedInterface() = default;

	virtual ~OtherDerivedInterface() { printf("~OtherInterface()\n"); }

	void Foo() override
	{
		printf("Something()\n");
	}
};

RegisterInterface(OtherDerivedInterface);