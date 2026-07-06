#pragma once

#include "ModuleRegistry.h"

#include "Core/Common.h"
#include "Core/Events/EventDispatcher.h"
#include "Core/Logging/Logging.h"
#include "Core/Profiling/Profiler.h"
#include "Core/Reflection/TypeId.h"

namespace Oyl
{
	class EventDispatcher;
	class ModuleRegistry;
}

namespace Oyl
{
	class OYL_CORE_API Module
	{
		friend ModuleRegistry;

	public:
		Module();

		virtual
		~Module();

#	pragma region Public Interface
		virtual
		Reflection::TypeId
		GetTypeId() const = 0;

		virtual
		std::string_view
		GetName() const = 0;

		virtual
		bool
		IsEnabled() { return m_enabled; }

		virtual
		void
		SetEnabled(bool a_value) { m_enabled = a_value; }

#	pragma endregion
#	pragma region Callback functions
		virtual
		void
		OnInit() {}

		virtual
		void
		OnUpdate() {}

		virtual
		void
		OnShutdown() {}
#	pragma endregion
#	pragma region Events
		template<typename TModule, typename TEvent>
		void
		RegisterEventListener(void (TModule::*a_fn)(const TEvent&))
		{
			constexpr auto eventId = Reflection::GetTypeId<TEvent>();
			constexpr auto listenerId = Reflection::GetTypeId<decltype(*this)>();

			TModule* obj = static_cast<TModule*>(this);

			EventDelegate::MemberFn<TModule> fn;
			std::memcpy(&fn, &a_fn, sizeof(fn));

			auto delegate = EventDelegate::Create<TModule>(obj, fn);

			m_eventDispatcher->Register(eventId, listenerId, std::move(delegate));
		}

		template<typename TModule, typename TEvent>
		void
		RegisterEventListener(void (TModule::*a_fn)(const TEvent&) const)
		{
			Traits::RemoveMemberFunctionConst_T<decltype(a_fn)> fn;
			std::memcpy(&fn, &a_fn, sizeof(fn));
			RegisterEventListener(fn);
		}

		template<typename TEvent>
			requires (std::is_base_of_v<Event, TEvent> && !std::is_same_v<Event, TEvent>)
		void
		PostEvent(const TEvent& a_event)
		{
			OYL_PROFILE_FUNCTION();

			constexpr auto eventId = Reflection::GetTypeId<TEvent>();
			m_eventDispatcher->Dispatch(eventId, a_event);
		}

		void
		PostEvent(const Event& a_event);

	private:
		void
		OnRegisterEventDispatcher(EventDispatcher* a_dispatcher);
#	pragma endregion

	private:
		bool m_enabled = true;

		EventDispatcher* m_eventDispatcher = nullptr;
	};
}

#define OYL_DECLARE_MODULE(...) OYL_MACRO_OVERLOAD(_OYL_DECLARE_MODULE, __VA_ARGS__)

#define _OYL_DECLARE_MODULE_2(_class_, _parent_) \
	OYL_FORCE_FORMAT_INDENT \
private: \
	using This = _class_; \
	using Super = _parent_; \
	friend ::Oyl::ModuleRegistry; \
public: \
	constexpr ::Oyl::Reflection::TypeId \
	GetTypeId() const override \
	{ \
		return ::Oyl::Reflection::GetTypeId<decltype(*this)>(); \
	} \
	\
	constexpr std::string_view \
	GetName() const override\
	{ \
		return ::Oyl::NameOf<decltype(*this)>(); \
	} \
	\
private: \
	OYL_FORCE_SEMICOLON

#define _OYL_DECLARE_MODULE_1(_class_) _OYL_DECLARE_MODULE_2(_class_, ::Oyl::Module)