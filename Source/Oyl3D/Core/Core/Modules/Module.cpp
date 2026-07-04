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
	Module::PostEvent(const Event& a_event)
	{
		OYL_PROFILE_FUNCTION();

		m_eventDispatcher->Dispatch(a_event.typeId, a_event);
	}

	void
	Module::OnRegisterEventDispatcher(EventDispatcher* a_dispatcher)
	{
		m_eventDispatcher = a_dispatcher;
	}
}
