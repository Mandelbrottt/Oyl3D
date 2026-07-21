#pragma once

#include "Event.h"

namespace Oyl
{
	class EventListener;
	class EventDispatcher;

	namespace Internal
	{
		OYL_CORE_API
		void
		AttachEventListenerToEventDispatcher(EventListener* a_eventListener, EventDispatcher* a_dispatcher);
	}

	class OYL_CORE_API EventListener
	{
		friend void Internal::AttachEventListenerToEventDispatcher(EventListener*, EventDispatcher*);

	public:
		virtual
		~EventListener();

		void
		RegisterEventListener(EventId a_eventId, const OnEventDelegate& a_delegate, int32 a_priority);

	#define _EVENT_LISTENER_REGISTER_FUNCTION(_cv_ref_noexcept_) \
		template<typename TListener, typename TEvent> \
		void \
		RegisterEventListener(void (TListener::*a_fn)(const TEvent&) _cv_ref_noexcept_, int32 a_priority = 0) \
		{ \
			constexpr auto eventId = Reflection::GetTypeId<TEvent>(); \
			\
			TListener* obj = static_cast<TListener*>(this); \
			\
			OnEventDelegate::MemberFnFromType<EventListener> fn; \
			std::memcpy(&fn, &a_fn, sizeof(fn)); \
			\
			auto delegate = OnEventDelegate::Create(obj, fn); \
			\
			RegisterEventListener(eventId, std::move(delegate), a_priority); \
		}

	_CLASS_DEFINE_CV_REF_NOEXCEPT(_EVENT_LISTENER_REGISTER_FUNCTION)
	#undef _EVENT_LISTENER_REGISTER_FUNCTION

		void
		UnregisterEventListener(EventId a_eventId, const OnEventDelegate& a_delegate);

	#define _EVENT_LISTENER_UNREGISTER_FUNCTION(_cv_ref_noexcept_) \
		template<typename TListener, typename TEvent> \
		void \
		UnregisterEventListener(void (TListener::*a_fn)(const TEvent&) _cv_ref_noexcept_) \
		{ \
			constexpr auto eventId = Reflection::GetTypeId<TEvent>(); \
			\
			TListener* obj = static_cast<TListener*>(this); \
			\
			OnEventDelegate::MemberFnFromType<EventListener> fn; \
			std::memcpy(&fn, &a_fn, sizeof(fn)); \
			\
			auto delegate = OnEventDelegate::Create(obj, fn); \
			\
			UnregisterEventListener(eventId, std::move(delegate)); \
		}

	_CLASS_DEFINE_CV_REF_NOEXCEPT(_EVENT_LISTENER_UNREGISTER_FUNCTION)
	#undef _EVENT_LISTENER_UNREGISTER_FUNCTION

		void
		PostEvent(const Event& a_event) const;

	private:
		void
		OnRegisterEventDispatcher(EventDispatcher* a_dispatcher);

	private:
		/**
		 * \brief Non-Owning pointer to EventDispatcher
		 */
		EventDispatcher* m_eventDispatcher = nullptr;
	};

	// Ensure that EventListener has a virtual destructor, and has a vtable
	static_assert(std::has_virtual_destructor_v<EventListener>);
}