#include <iostream>
#include <cassert>

#include "Core/Interface.h"

class DerivedInterface : public Interface
{
public:
	DerivedInterface() = default;
	virtual ~DerivedInterface() { printf("Destructor\n"); };

	void Foo() override
	{
		printf("Bar\n");
	}
};

extern "C"
{
	__declspec(dllexport) size_t InterfaceSize()
	{
		return sizeof(DerivedInterface);
	}

	__declspec(dllexport) Interface* AllocInterface(void* a_location, size_t a_sizeInBytes)
	{
		assert(a_sizeInBytes == InterfaceSize());

		DerivedInterface* interfaceLocation = reinterpret_cast<DerivedInterface*>(a_location);

		new(interfaceLocation) DerivedInterface;

		return interfaceLocation;
	}
}
