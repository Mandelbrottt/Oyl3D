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

#define OYL_DECLARE_MODULE(_class_, _name_) \
public: \
	using This = _class_;\
	using Super = ::Oyl::Module;\
private: \
	friend class ::Oyl::ModuleRegistry;\
	_class_() \
		: ::Oyl::Module(_name_) {} \
	\
	::Oyl::TypeId \
	GetTypeId() override \
	{ \
		return ::Oyl::GetTypeId<_class_>(); \
	} \
	OYL_FORCE_SEMICOLON