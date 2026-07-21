#include "Assembly.h"

#include "ReflectionFactory.h"

namespace Oyl::Reflection
{
	static std::vector<Assembly*> g_assemblies;
	static std::unordered_map<std::string_view, Assembly*> g_assemblyMap;

	Assembly::Assembly(const Internal::AssemblyParams& a_params)
		: m_name(a_params.name) {}

	Assembly::~Assembly() {}

	const std::vector<Type*>&
	Assembly::GetTypes() const
	{
		return m_types;
	}

	Type*
	Assembly::GetType(std::string_view a_typeName) const
	{
		auto iter = m_typeMap.find(a_typeName);
		if (iter == m_typeMap.end())
			return nullptr;
		return iter->second;
	}
	
	const std::vector<Variable*>&
	Assembly::GetGlobalVariables() const
	{
		return m_globalVariables;
	}

	Variable*
	Assembly::GetGlobalVariable(std::string_view a_variableName) const
	{
		auto iter = m_globalVariableMap.find(a_variableName);
		if (iter == m_globalVariableMap.end())
			return nullptr;
		return iter->second;
	}

	const std::vector<Function*>&
	Assembly::GetGlobalFunctions() const
	{
		return m_globalFunctions;
	}

	Function*
	Assembly::GetGlobalFunction(std::string_view a_functionName) const
	{
		auto iter = m_globalFunctionMap.find(a_functionName);
		if (iter == m_globalFunctionMap.end())
			return nullptr;
		return iter->second;
	}

	const std::vector<Enum*>&
	Assembly::GetEnums() const
	{
		return m_enums;
	}

	Enum*
	Assembly::GetEnum(std::string_view a_enumName) const
	{
		auto iter = m_enumMap.find(a_enumName);
		if (iter == m_enumMap.end())
			return nullptr;
		return iter->second;
	}

	const std::vector<Assembly*>&
	Assembly::GetAssemblies()
	{
		return g_assemblies;
	}

	Assembly*
	Assembly::GetAssembly(std::string_view a_assemblyName)
	{
		auto iter = g_assemblyMap.find(a_assemblyName);
		if (iter == g_assemblyMap.end())
			return nullptr;
		return iter->second;
	}

	void
	Assembly::AddType(Type* a_type)
	{
		m_types.emplace_back(a_type);
	}

	void
	Assembly::AddGlobalVariable(Variable* a_variable)
	{
		m_globalVariables.emplace_back(a_variable);
	}

	void
	Assembly::AddGlobalFunction(Function* a_function)
	{
		m_globalFunctions.emplace_back(a_function);
	}

	void
	Assembly::AddEnum(Enum* a_enum)
	{
		m_enums.emplace_back(a_enum);
	}

	void
	Assembly::SetupAssemblyMaps()
	{
		m_typeMap.reserve(m_types.size());
		for (size_t i = 0; i < m_types.size(); i++)
		{
			auto& type = m_types[i];
			m_typeMap.emplace(type->GetQualifiedName(), type);
		}

		m_globalVariableMap.reserve(m_globalVariables.size());
		for (size_t i = 0; i < m_globalVariables.size(); i++)
		{
			auto& variable = m_globalVariables[i];
			m_globalVariableMap.emplace(variable->GetQualifiedName(), variable);
		}

		m_globalFunctionMap.reserve(m_globalFunctions.size());
		for (size_t i = 0; i < m_globalFunctions.size(); i++)
		{
			auto& function = m_globalFunctions[i];
			m_globalFunctionMap.emplace(function->GetQualifiedName(), function);
		}
	}
}
