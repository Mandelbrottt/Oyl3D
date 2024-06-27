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
		GetTypeId() const = 0;
	};

	using OnEventFn = std::function<void(Event&)>;

	class EventDispatcher
	{
	public:
		explicit
		EventDispatcher(Event& a_event)
			: event { a_event } {}

		Event& event;

		void
		Dispatch(TypeId a_type, OnEventFn a_fn) const
		{
			if (a_type == event.GetTypeId())
			{
				a_fn(event);
			}
		}
	};
}

#define OYL_DECLARE_EVENT(...) OYL_MACRO_OVERLOAD(_OYL_DECLARE_EVENT, __VA_ARGS__)

#define _OYL_DECLARE_EVENT_1(_event_) \
	static \
	::Oyl::TypeId \
	GetStaticTypeId() \
	{ \
		return ::Oyl::GetTypeId<_event_>();\
	} \
	\
	::Oyl::TypeId \
	GetTypeId() const override \
	{ \
		return GetStaticTypeId();\
	} \
	OYL_FORCE_SEMICOLON
