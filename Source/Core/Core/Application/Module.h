#pragma once

#include "Core/Common.h"
#include "Core/Types/TypeId.h"

namespace Oyl
{
	class OYL_CORE_API Module
	{
	public:
		explicit
		Module(std::string a_name = "Module");
		
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
		bool m_enabled;
		
		std::string m_debugName;
	};
}
