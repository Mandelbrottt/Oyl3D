#include <cstdio>

#include "Core/Module/ModuleManager.h"

class TestModuleInterface : public Oyl::IModuleInterface
{
public:
	TestModuleInterface()
	{
		puts("New TestModule!");
	}

	virtual
	~TestModuleInterface()
	{
		puts("Delete TestModule!");
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