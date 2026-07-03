#pragma once

#include "ModuleRegistry.h"

#include "Core/Common.h"
#include "Core/Profiling/Profiler.h"
#include "Core/Reflection/TypeId.h"

namespace Oyl
{
	class OYL_CORE_API Module
	{
	public:
		Module() {}

		virtual
		~Module() = default;

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
		void
		SetOnPostEventCallback(Detail::OnEventDelegate a_fn) { m_onPostEventCallback = a_fn; }

		template<typename TModule, typename TEvent>
		void
		RegisterEventListener(void (TModule::*a_fn)(TEvent&))
		{
			// Ensure ABI is the same between the incoming member function and the
			// output member function, then type coerce
			using OnEventFn = void(Module::*)(Event&);
			static_assert(sizeof(a_fn) == sizeof(OnEventFn));

			m_eventFns[TEvent::GetStaticTypeId()] =
				std::bind(
					*reinterpret_cast<OnEventFn*>(&a_fn),
					this,
					std::placeholders::_1
				);
		}

		template<typename TModule, typename TEvent>
		void
		RegisterEventListener(void (TModule::*a_fn)(TEvent&) const)
		{
			RegisterEventListener(const_cast<std::decay_t<decltype(a_fn)>>(a_fn));
		}

		// TODO: Add event to global event queue, arena?
		template<typename TEvent>
		void
		PostEvent(TEvent a_event)
		{
			OYL_PROFILE_FUNCTION();
			m_onPostEventCallback(a_event);
		}

		void
		OnEvent(Event& a_event);
#	pragma endregion

	private:
		bool m_enabled = true;

		Detail::OnEventDelegate m_onPostEventCallback;

		std::unordered_map<Reflection::TypeId, Detail::OnEventDelegate> m_eventFns;
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
