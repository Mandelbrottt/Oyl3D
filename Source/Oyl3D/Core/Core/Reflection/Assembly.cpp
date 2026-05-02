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

	const std::vector<Function*>&
	Assembly::GetFreeFunctions() const
	{
		return m_functions;
	}

	Type*
	Assembly::GetType(std::string_view a_typeName) const
	{
		auto iter = m_typeMap.find(a_typeName);
		if (iter == m_typeMap.end())
			return nullptr;
		return iter->second;
	}

	Function*
	Assembly::GetFunction(std::string_view a_functionName) const
	{
		auto iter = m_functionMap.find(a_functionName);
		if (iter == m_functionMap.end())
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
	Assembly::AddFunction(Function* a_function)
	{
		m_functions.emplace_back(a_function);
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

		m_functionMap.reserve(m_functions.size());
		for (size_t i = 0; i < m_functions.size(); i++)
		{
			auto& function = m_functions[i];
			m_functionMap.emplace(function->GetQualifiedName(), function);
		}
	}
}
