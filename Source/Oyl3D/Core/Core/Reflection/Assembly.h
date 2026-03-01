#pragma once

#include "Type.h"

namespace Oyl::Reflection
{
	namespace Detail
	{
		using AssembliesContainer = std::vector<Assembly*>;
		using OwnedTypesContainer = std::vector<Type*>;
	}

	static
	Detail::AssembliesContainer&
	Assemblies()
	{
		static Detail::AssembliesContainer container {};
		return container;
	}
	
	class OYL_CORE_API Assembly
	{
	public:
		bool
		RegisterType(Type* a_type);
		
		bool
		UnRegisterType(Type* a_type);
		
	private:
		std::string m_name;

		Detail::OwnedTypesContainer m_ownedTypes;
	};
}