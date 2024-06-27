#pragma once

#include "Core/Types/TypeId.h"

namespace Oyl
{
	struct Event
	{
		virtual
		~Event() = default;
	
		virtual
		TypeId
		GetTypeId() = 0;
	};
}
	
#define OYL_DECLARE_EVENT(_event_) \
	static \
	::Oyl::TypeId \
	GetStaticTypeId() \
	{ \
		return ::Oyl::GetTypeId<_event_>();\
	} \
	\
	::Oyl::TypeId \
	GetTypeId() override \
	{ \
		return GetStaticTypeId();\
	}	

