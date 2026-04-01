#pragma once

#include "Field.h"
#include "Function.h"
#include "Type.h"

namespace Spyll::Reflection
{
	class Assembly final
	{
		friend AssemblyFactory;

		Assembly();

	public:
		~Assembly();

		const std::vector<Type>&
		GetTypes() const;

		const Type*
		GetType(std::string_view a_typeName) const;

		const std::vector<Function>&
		GetFreeFunctions() const;

		const Function*
		GetFunction(std::string_view a_functionName) const;

	public:
		static
		const std::vector<Assembly*>&
		GetAssemblies();

		static
		const Assembly*
		GetAssembly(std::string_view a_assemblyName);

	private:
		void
		SetupAssemblyMaps();

	private:
		static std::vector<Assembly*> s_assemblies;
		static std::unordered_map<std::string_view, Assembly*> s_assemblyMap;

		std::vector<Type> m_types;
		std::unordered_map<std::string_view, Type*> m_typeMap;

		std::vector<Function> m_functions;
		std::unordered_map<std::string_view, Function*> m_functionMap;
	};
}
