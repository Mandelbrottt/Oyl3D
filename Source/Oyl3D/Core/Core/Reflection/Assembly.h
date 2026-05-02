#pragma once

#include "Field.h"
#include "Function.h"
#include "Type.h"

namespace Oyl::Reflection::Internal {
	struct AssemblyParams;
}

namespace Oyl::Reflection
{
	namespace Internal
	{
		class ReflectionFactory;
	}

	class Assembly final
	{
		friend Internal::ReflectionFactory;

		explicit
		Assembly(const Internal::AssemblyParams& a_params);

	public:
		~Assembly();

		const std::vector<Type*>&
		GetTypes() const;

		Type*
		GetType(std::string_view a_typeName) const;

		const std::vector<Function*>&
		GetFreeFunctions() const;

		Function*
		GetFunction(std::string_view a_functionName) const;

	public:
		static
		const std::vector<Assembly*>&
		GetAssemblies();

		static
		Assembly*
		GetAssembly(std::string_view a_assemblyName);

	private:
		void
		AddType(Type* a_type);
		
		void
		AddFunction(Function* a_function);

		void
		SetupAssemblyMaps();

	private:
		std::string_view m_name;

		std::vector<Type*> m_types;
		std::unordered_map<std::string_view, Type*> m_typeMap;

		std::vector<Function*> m_functions;
		std::unordered_map<std::string_view, Function*> m_functionMap;
	};
}
