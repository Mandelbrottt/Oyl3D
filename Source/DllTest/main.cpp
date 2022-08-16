#include <iostream>
#include <cassert>
#include <vector>

#include "Core/Interface.h"

static std::vector<std::string> g_interfaces;

extern "C" __declspec(dllexport) std::vector<std::string> const& DllTest_Interfaces()
{
	return g_interfaces;
}

#define RegisterInterface(_class_) \
	static int IIFE##_class_ = [] \
	{ \
		g_interfaces.push_back(#_class_); \
		return 0; \
	}(); \
	extern "C" __declspec(dllexport) size_t _class_##Size() \
	{ \
		return sizeof(_class_); \
	} \
	extern "C" __declspec(dllexport) Interface* Alloc##_class_(void* a_location, size_t a_sizeInBytes) \
	{ \
		assert(a_sizeInBytes == _class_##Size()); \
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