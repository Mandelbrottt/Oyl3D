#include "pch.h"

#include "ReflectionTest.h"

#include "Core/Logging/Logging.h"

Test::Something::Something()
{
	OYL_UNUSED(m_somethingElse);
	OYL_LOG("Something()");
}

Test::Something::~Something()
{
	OYL_LOG("~Something()");
}

void
Test::Something::OnUpdate() {}

void
Test::Something::DoSomething()
{
	OYL_LOG("DoSomething()");
}
