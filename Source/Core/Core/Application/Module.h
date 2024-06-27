#pragma once

#include "Core/Common.h"
#include "Core/Types/TypeId.h"
#include "ModuleRegistry.h"

namespace Oyl
{
	class OYL_CORE_API Module
	{
	public:
		Module();
		
		virtual
		~Module() = default;

#	pragma region Public Interface
		virtual
		TypeId
		GetTypeId() = 0;

		template<typename TModule, typename... TArgs, std::enable_if_t<std::is_base_of_v<TModule, Module>> = true>
		Module*
		Register(TArgs&&... a_args);
		
		template<typename TModule, std::enable_if_t<std::is_base_of_v<TModule, Module>> = true>
		Module*
		Get();
		
		template<typename TModule, std::enable_if_t<std::is_base_of_v<TModule, Module>> = true>
		bool
		Remove();
		 
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

	private:
		bool m_enabled = true;
	};
}

#define OYL_DECLARE_MODULE(...) OYL_MACRO_OVERLOAD(_OYL_DECLARE_MODULE, __VA_ARGS__)

#define _OYL_DECLARE_MODULE_3(_class_, _parent_, _name_) \
	OYL_FORCE_FORMAT_INDENT\
private: \
	using This = _class_; \
	using Super = _parent_; \
	friend class ::Oyl::ModuleRegistry; \
public: \
	::Oyl::TypeId \
	GetTypeId() override \
	{ \
		return ::Oyl::GetTypeId<_class_>(); \
	} \
	\
	std::string_view \
	GetName() const { return _name_ } \
	\
	static \
	_class_* \
	Get() { return ::Oyl::Module::Get<_class_>(); } \
	OYL_FORCE_SEMICOLON

#define _OYL_DECLARE_MODULE_2(_class_, _name_) _OYL_DECLARE_MODULE_3(_class_, ::Oyl::Module, _name_)