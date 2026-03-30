#include "Type.h"

namespace Spyll::Reflection
{
	struct Type::Impl
	{
		TypeId typeId;

		uint32_t size;
		
		std::string name;
		std::string qualifiedName;

		bool isPrimitive;

		Assembly* assembly;

		std::vector<Type*> baseTypes;

		std::vector<Function*> functions;
		std::vector<Field*> fields;
	};

	Type::Type()
	{
		m_impl = new Impl;
	}

	Type::~Type()
	{
		delete m_impl;
		m_impl = nullptr;
	}

	TypeId
	Type::GetTypeId() const
	{
		return m_impl->typeId;
	}

	std::string_view
	Type::GetName() const
	{
		return m_impl->name;
	}

	std::string_view
	Type::GetQualifiedName() const
	{
		return m_impl->qualifiedName;
	}

	uint32_t
	Type::GetSize() const
	{
		return m_impl->size;
	}

	bool
	Type::IsPrimitive() const
	{
		return m_impl->isPrimitive;
	}

	const std::vector<Type*>&
	Type::GetBaseTypes() const
	{
		return m_impl->baseTypes;
	}

	const std::vector<Field*>&
	Type::GetFields() const
	{
		return m_impl->fields;
	}

	const std::vector<Function*>&
	Type::GetFunctions() const
	{
		return m_impl->functions;
	}
}
