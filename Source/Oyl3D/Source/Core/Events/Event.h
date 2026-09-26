#pragma once

#include "Core/Delegate.h"
#include "Core/Reflection/TypeId.h"

namespace Oyl
{
	using EventId = Reflection::TypeId;

	struct Event
	{
		friend class EventDispatcher;

		const EventId eventType;

		virtual
		~Event() = default;

		/**
		 * \brief Cancel the event currently being handled.
		 *        This prevents lower priority listeners from consuming the event
		 */
		void
		Cancel() const
		{
			m_cancelled = true;
		}

	protected:
		explicit
		constexpr
		Event(EventId a_eventType)
			: eventType(a_eventType) {}

	private:
		mutable bool m_cancelled = false;
	};

	template<typename TEvent>
	struct EventBase : Event
	{
	protected:
		constexpr
		EventBase()
			: Event(Reflection::GetTypeId<TEvent>()) {}
	};

	using OnEventSignature = void(const Event&);
	using OnEventDelegate = Delegate<OnEventSignature>;

	using PostEventSignature = void(const Event&);
	using PostEventDelegate = Delegate<PostEventSignature>;
}