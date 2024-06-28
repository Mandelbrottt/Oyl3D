#include "pch.h"
#include "Module.h"

namespace Oyl
{
	void Module::OnEvent(Event& a_event)
	{
		OYL_PROFILE_FUNCTION();
		EventDispatcher dispatcher(a_event);
		for (auto& [type, fn] : m_eventFns)
		{
			dispatcher.Dispatch(type, fn);
		}
	}
}
