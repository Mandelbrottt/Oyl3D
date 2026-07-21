#pragma once

#include "Enum.h"
#include "Function.h"
#include "ReflectionParams.h"
#include "Type.h"

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

		const std::vector<Variable*>&
		GetGlobalVariables() const;

		Variable*
		GetGlobalVariable(std::string_view a_variableName) const;

		const std::vector<Function*>&
		GetGlobalFunctions() const;

		Function*
		GetGlobalFunction(std::string_view a_functionName) const;

		const std::vector<Enum*>&
		GetEnums() const;

		Enum*
		GetEnum(std::string_view a_enumName) const;

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
		AddGlobalVariable(Variable* a_variable);

		void
		AddGlobalFunction(Function* a_function);

		void
		AddEnum(Enum* a_enum);

		void
		SetupAssemblyMaps();

	private:
		std::string_view m_name;

		std::vector<Type*> m_types;
		std::unordered_map<std::string_view, Type*> m_typeMap;

		std::vector<Variable*> m_globalVariables;
		std::unordered_map<std::string_view, Variable*> m_globalVariableMap;

		std::vector<Function*> m_globalFunctions;
		std::unordered_map<std::string_view, Function*> m_globalFunctionMap;

		std::vector<Enum*> m_enums;
		std::unordered_map<std::string_view, Enum*> m_enumMap;
	};
}
