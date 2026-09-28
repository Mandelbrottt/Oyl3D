#include <cstdio>

#include "Core/Module/ModuleManager.h"

class TestModuleInterface : public Oyl::IModuleInterface
{
public:
	TestModuleInterface()
	{
		printf("New TestModule!\n");
	}

	virtual
	~TestModuleInterface()
	{
		printf("Delete TestModule!\n");
	}
};

extern "C"
__OYL_EXPORT_ATTR
Oyl::IModuleInterface*
Test_NewModuleInterface()
{
	return new TestModuleInterface();
}

extern "C"
__OYL_EXPORT_ATTR
void
Test_DeleteModuleInterface(TestModuleInterface* a_interface)
{
	delete a_interface;
}