#include "Assembly.h"

//extern "C"
//{
	//void
	//_PopulateReflectionAssembly(Spyll::Reflection::Assembly* a_assembly)
	//{
	//	using namespace Spyll::Reflection;

	//	Type type;
	//	type.m_assembly = a_assembly;
	//}
//}

namespace Spyll::Reflection
{
	std::vector<Assembly*> Assembly::s_assemblies;
	std::unordered_map<std::string_view, Assembly*> Assembly::s_assemblyMap;

	Assembly::Assembly() {}

	Assembly::~Assembly() {}

	const std::vector<Type>&
	Assembly::GetTypes() const
	{
		return m_types;
	}
	
	const std::vector<Function>&
	Assembly::GetFreeFunctions() const
	{
		return m_functions;
	}

	const Type*
	Assembly::GetType(std::string_view a_typeName) const
	{
		auto iter = m_typeMap.find(a_typeName);
		if (iter == m_typeMap.end())
			return nullptr;
		return iter->second;
	}

	const Function*
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
		return s_assemblies;
	}

	const Assembly*
	Assembly::GetAssembly(std::string_view a_assemblyName)
	{
		auto iter = s_assemblyMap.find(a_assemblyName);
		if (iter == s_assemblyMap.end())
			return nullptr;
		return iter->second;
	}

	void
	Assembly::SetupAssemblyMaps()
	{
		m_typeMap.reserve(m_types.size());
		for (size_t i = 0; i < m_types.size(); i++)
		{
			auto& type = m_types[i];
			m_typeMap.emplace(type.GetQualifiedName(), &type);
		}

		m_functionMap.reserve(m_functions.size());
		for (size_t i = 0; i < m_functions.size(); i++)
		{
			auto& function = m_functions[i];
			m_functionMap.emplace(function.GetQualifiedName(), &function);
		}
	}
}
