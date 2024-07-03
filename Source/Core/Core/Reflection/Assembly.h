#pragma once

#include "Type.h"

namespace Oyl::Reflection
{
	class OYL_CORE_API Assembly
	{
	public:
		using AssembliesContainer = std::vector<std::shared_ptr<Assembly>>;
		using OwnedTypesContainer = std::vector<std::shared_ptr<Type>>;
		
		static
		AssembliesContainer&
		Assemblies()
		{
			static AssembliesContainer container {};
			return container;
		}

		bool
		RegisterType(std::shared_ptr<Type>& a_type);
		
		bool
		UnRegisterType(std::shared_ptr<Type>& a_type);
		
	private:
		std::string m_name;

		OwnedTypesContainer m_ownedTypes;
	};
}