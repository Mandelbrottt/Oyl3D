#pragma once

#include "Type.h"

namespace Oyl::Reflection
{
	class OYL_CORE_API Assembly
	{
	public:
		using AssembliesContainer = std::vector<Assembly*>;
		using OwnedTypesContainer = std::vector<Type*>;
		
		static
		AssembliesContainer&
		Assemblies()
		{
			static AssembliesContainer container {};
			return container;
		}

		bool
		RegisterType(Type* a_type);
		
		bool
		UnRegisterType(Type* a_type);
		
	private:
		std::string m_name;

		OwnedTypesContainer m_ownedTypes;
	};
}