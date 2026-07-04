#include "pch.h"
#include "Module.h"

namespace Oyl
{
	Module::Module() {}

	Module::~Module()
	{
		m_eventDispatcher = nullptr;
	}

	void
	Module::OnRegisterEventDispatcher(EventDispatcher* a_dispatcher)
	{
		m_eventDispatcher = a_dispatcher;
	}
}
