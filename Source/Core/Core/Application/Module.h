#pragma once

#include "ModuleRegistry.h"

#include "Core/Common.h"
#include "Core/Profiling/Profiler.h"
#include "Core/Types/TypeId.h"

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
		TypeId
		GetTypeId() = 0;

		template<typename TModule, typename... TArgs, ModuleRegistry::enable_if_base_of_module_t<TModule> = true>
		static
		TModule*
		Register(TArgs&&... a_args)
		{
			TModule*        module   = new TModule();
			ModuleRegistry* registry = ModuleRegistry::Instance();
			registry->RegisterModule(module);
			return module;
		}

		template<typename TModule, ModuleRegistry::enable_if_base_of_module_t<TModule> = true>
		static
		TModule*
		Get()
		{
			ModuleRegistry* registry = ModuleRegistry::Instance();
			return reinterpret_cast<TModule*>(registry->GetModule(TModule::GetStaticTypeId()));
		}

		template<typename TModule, ModuleRegistry::enable_if_base_of_module_t<TModule> = true>
		static
		bool
		Remove()
		{
			ModuleRegistry* registry = ModuleRegistry::Instance();
			return registry->RemoveModule(TModule::GetStaticTypeId());
		}

		virtual
		bool
		IsEnabled() { return m_enabled; }

		virtual
		void
		SetEnabled(bool a_value) { m_enabled = a_value; }

		virtual
		std::string_view
		GetName() const = 0;
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
		SetOnPostEventCallback(OnEventFn a_fn) { m_onPostEventCallback = a_fn; }

		template<typename TModule, typename TEvent>
		void
		RegisterEvent(void (TModule::*a_fn)(TEvent&))
		{
			m_eventFns[TEvent::GetStaticTypeId()] =
				std::bind(
					reinterpret_cast<void(Module::*)(Event&)>(a_fn),
					this,
					std::placeholders::_1
				);
		}

		template<typename TModule, typename TEvent>
		void
		RegisterEvent(void (TModule::*a_fn)(TEvent&) const)
		{
			RegisterEvent(const_cast<std::decay_t<decltype(a_fn)>>(a_fn));
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

		OnEventFn m_onPostEventCallback;

		std::unordered_map<TypeId, OnEventFn> m_eventFns;
	};
}

#define OYL_DECLARE_MODULE(...) OYL_MACRO_OVERLOAD(_OYL_DECLARE_MODULE, __VA_ARGS__)

#define _OYL_DECLARE_MODULE_3(_class_, _parent_, _name_) \
	OYL_FORCE_FORMAT_INDENT \
private: \
	using This = _class_; \
	using Super = _parent_; \
	friend ::Oyl::ModuleRegistry; \
public: \
	static \
	::Oyl::TypeId \
	GetStaticTypeId() \
	{ \
		return ::Oyl::GetTypeId<_class_>(); \
	} \
	::Oyl::TypeId \
	GetTypeId() override \
	{ \
		\
		static_assert( \
			std::is_same_v<_class_, std::decay_t<decltype(*this)>>, \
			"Named class \"" #_class_ "\" does not match class in which it is declared!" \
		); \
		return GetStaticTypeId(); \
	} \
	\
	std::string_view \
	GetName() const { return _name_; } \
	\
	template<typename... TArgs>\
	static \
	_class_* \
	Register(TArgs&&... a_args) { return ::Oyl::Module::Register<_class_>(std::forward<TArgs>(a_args)...); } \
	\
	static \
	_class_* \
	Get() { return ::Oyl::Module::Get<_class_>(); } \
	\
	static \
	bool \
	Remove() { return ::Oyl::Module::Remove<_class_>(); } \
private: \
	OYL_FORCE_SEMICOLON

#define _OYL_DECLARE_MODULE_1(_class_) _OYL_DECLARE_MODULE_3(_class_, ::Oyl::Module, #_class_)
#define _OYL_DECLARE_MODULE_2(_class_, _name_) _OYL_DECLARE_MODULE_3(_class_, ::Oyl::Module, _name_)
