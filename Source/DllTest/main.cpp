#include <iostream>
#include <cassert>
#include <vector>

#include <Core/Interface.h>

static std::vector<std::string> g_interfaces;

// ReSharper disable once CppInconsistentNaming
extern "C" __declspec(dllexport) const std::vector<std::string>* __DllTest_Interfaces()
{
	return &g_interfaces;
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

void Foo();

class DerivedInterface : public Interface
{
public:
	DerivedInterface() = default;

	virtual ~DerivedInterface() { printf("~DerivedInterface()\n"); }

	void Foo() override
	{
		printf("Foo()\n");
		::Foo();
	}
};

RegisterInterface(DerivedInterface);

class IBase
{
	void Bar() {}
};


class OtherDerivedInterface : public DerivedInterface, public IBase
{
public:
	OtherDerivedInterface() = default;

	virtual ~OtherDerivedInterface() { printf("~OtherInterface()\n"); }

	void Foo() override
	{
		printf("ODI %llu\n", something);
	}
	
	constexpr static size_t something  = sizeof(void (OtherDerivedInterface::*)());
};

RegisterInterface(OtherDerivedInterface);

class Nothing
{
public:
	void Foo() {}
};

void Foo()
{
	printf("%zu %zu\n", sizeof(&Nothing::Foo), sizeof(&Interface::Foo));
	
	auto ptrToMember = &OtherDerivedInterface::Foo;
	printf("\t%llu\n", sizeof(ptrToMember));

	struct PtrToMember
	{
		void* basePtr;
		void* adjustor;
	};

	PtrToMember ptrToMemberStruct {};

	memcpy(&ptrToMemberStruct, &ptrToMember, sizeof(ptrToMember));

	void (OtherDerivedInterface::*thereAndBack)();

	memcpy(&thereAndBack, &ptrToMemberStruct, sizeof(thereAndBack));
	(OtherDerivedInterface().*thereAndBack)();
	
	printf("\t%p %p\n", ptrToMemberStruct.basePtr, ptrToMemberStruct.adjustor);

	auto ptrToMemberCast = [](void (OtherDerivedInterface::* a_src)())->PtrToMember
	{
		PtrToMember result {};
		assert(sizeof(result) == sizeof(a_src));
		memcpy(&result, &a_src, sizeof(a_src));
		return result;
	};

	auto ptms = ptrToMemberCast(&OtherDerivedInterface::Foo);
	printf("\t%p %p\n", ptms.basePtr, ptms.adjustor);
	
	memcpy(&thereAndBack, &ptms, sizeof(thereAndBack));
	(OtherDerivedInterface().*thereAndBack)();
}