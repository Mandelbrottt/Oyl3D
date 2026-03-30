#include "Type.h"

#include "Assembly.h"

namespace Spyll::Reflection
{
	Type::Type() {}

	Type::~Type() {}

	TypeId
	Type::GetTypeId() const
	{
		return m_typeId;
	}

	std::string_view
	Type::GetName() const
	{
		return m_name;
	}

	std::string
	Type::GetQualifiedName() const
	{
		return m_qualifiedName;
	}

	uint32_t
	Type::GetSize() const
	{
		return m_size;
	}

	bool
	Type::IsPrimitive() const
	{
		return m_isPrimitive;
	}

	const std::vector<Type*>&
	Type::GetBaseTypes() const
	{
		return m_baseTypes;
	}

	const std::vector<Field*>&
	Type::GetFields() const
	{
		return m_fields;
	}

	const std::vector<Function*>&
	Type::GetFunctions() const
	{
		return m_functions;
	}

	const Type*
	Type::Get(std::string_view a_name)
	{
		auto& assemblies = Assembly::GetAssemblies();

		for (auto& [name, assembly] : assemblies)
		{
			if (assembly->)
		}
	}
}
