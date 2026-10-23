#pragma once

#include "Core/Containers/Delegate.h"
#include "Core/Types/TypeId.h"

namespace Oyl
{
	using EventId = TypeId;

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
			: Event(typeidof(TEvent)) {}
	};

	template<typename TEvent>
		requires Traits::ConvertibleTo<TEvent&, Event&>
	using OnEventSignature = void(const TEvent&);

	template<typename TEvent>
		requires Traits::ConvertibleTo<TEvent&, Event&>
	using OnEventDelegate = Delegate<OnEventSignature<TEvent>>;

	template<typename TEvent>
		requires Traits::ConvertibleTo<TEvent&, Event&>
	using PostEventSignature = void(const TEvent&);

	template<typename TEvent>
		requires Traits::ConvertibleTo<TEvent&, Event&>
	using PostEventDelegate = Delegate<PostEventSignature<TEvent>>;
}