#include "pch.h"
#include "Module.h"

namespace Oyl
{
	void Module::OnEvent(Event& a_event)
	{
		EventDispatcher dispatcher(a_event);
		for (auto& [type, fn] : m_eventFns)
		{
			dispatcher.Dispatch(type, fn);
		}
	}
}
