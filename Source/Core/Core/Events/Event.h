#pragma once

#include <functional>

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

	namespace Detail
	{
		using OnEventDelegate = std::function<void(Event&)>;

		class EventDispatcher
		{
		public:
			explicit
			EventDispatcher(Event& a_event)
				: event { a_event } {}

			Event& event;

			void
			Dispatch(TypeId a_type, OnEventDelegate a_fn) const
			{
				if (a_type == event.GetTypeId())
				{
					a_fn(event);
				}
			}
		};
	}
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
		static_assert( \
			std::is_same_v<_event_, std::decay_t<decltype(*this)>>, \
			"Named class \"" #_event_ "\" does not match class in which it is declared!" \
		); \
		return GetStaticTypeId();\
	} \
	OYL_FORCE_SEMICOLON
