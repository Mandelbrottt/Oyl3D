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

	void OnStartModule() override
	{
		puts("OnStart TestModule");
	}

	void OnStopModule() override
	{
		puts("OnStop TestModule");
	}
};

extern "C"
__OYL_EXPORT_ATTR
Oyl::IModuleInterface*
InitModule_Test()
{
	return new TestModuleInterface();
}
