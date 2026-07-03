#pragma once

#include <functional>

#include "Core/Reflection/TypeId.h"

namespace Oyl
{
	struct Event
	{
		virtual
		~Event() = default;

		virtual
		Reflection::TypeId
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
			Dispatch(Reflection::TypeId a_type, OnEventDelegate a_fn) const
			{
				if (a_type == event.GetTypeId())
				{
					a_fn(event);
				}
			}
		};
	}
}

#define OYL_DECLARE_EVENT() \
	constexpr ::Oyl::Reflection::TypeId \
	GetTypeId() const override \
	{ \
		return ::Oyl::Reflection::GetTypeId<decltype(*this)>();\
	} \
	OYL_FORCE_SEMICOLON
