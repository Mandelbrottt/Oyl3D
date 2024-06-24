#pragma once

#include "Core/Common.h"
#include "Core/Types/TypeId.h"

namespace Oyl
{
	class OYL_CORE_API Module
	{
	public:
		explicit
		Module(std::string a_name);
		
		virtual
		~Module() = default;

		virtual
		TypeId
		GetTypeId() = 0;
		
		virtual
		void
		OnInit() {}

		virtual
		void
		OnUpdate() {}

		virtual
		void
		OnShutdown() {}
		 
		virtual
		bool
		IsEnabled() { return m_enabled; }

		virtual
		void
		SetEnabled(bool a_value) { m_enabled = a_value; }
		
	private:
		bool m_enabled = true;
		
		std::string m_debugName;
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