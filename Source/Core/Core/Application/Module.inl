#pragma once

#include "Module.h"

namespace Oyl
{
	template<typename TModule, typename TEvent>
	void
	Module::RegisterEvent(void (TModule::*a_fn)(TEvent&))
	{
		m_eventFns[TEvent::GetStaticTypeId()] =
			std::bind(
				reinterpret_cast<void(Module::*)(Event&)>(a_fn),
				this,
				std::placeholders::_1
			);
	}

	template<typename TEvent>
	void Module::PostEvent(TEvent a_event)
	{
		m_onPostEventCallback(a_event);
	}
}
