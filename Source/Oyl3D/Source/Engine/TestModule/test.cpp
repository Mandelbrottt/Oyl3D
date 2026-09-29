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

OYL_MODULE_DECLARE(Test, TestModuleInterface)