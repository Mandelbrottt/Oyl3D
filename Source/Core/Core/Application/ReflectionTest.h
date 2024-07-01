#pragma once

#include "Core/Reflection/Reflection.h"

class Something
{
	ReflectDeclare(Something);

public:
	Something()
	{
		OYL_LOG("Something()");
	}

	virtual
	~Something()
	{
		OYL_LOG("~Something()");
	}

	void
	DoSomething()
	{
		OYL_LOG("DoSomething()");
	}

	int something = 5;
};
