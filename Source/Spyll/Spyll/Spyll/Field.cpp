#include "Field.h"

#include "Type.h"

namespace Spyll::Reflection
{
	Field::Field() {}

	Field::~Field() {}

	std::string_view
	Field::GetName() const
	{
		return m_name;
	}

	std::string_view
	Field::GetQualifiedName() const
	{
		return m_qualifiedName;
	}

	const Type*
	Field::GetType() const
	{
		return m_type;
	}

	const Type*
	Field::GetOwningType() const
	{
		return m_ownerType;
	}

	uint32_t
	Field::GetOffset() const
	{
		return m_offset;
	}

	AccessSpecifier
	Field::GetAccessSpecifier() const
	{
		return m_accessSpecifier;
	}

	bool
	Field::IsConst() const
	{
		return m_isConst;
	}

	bool
	Field::IsVolatile() const
	{
		return m_isVolatile;
	}

	bool
	Field::IsPointer() const
	{
		return m_isPointer;
	}

	bool
	Field::IsReference() const
	{
		return m_isReference;
	}

	bool
	Field::GetValue(void* a_self, void** a_outPtr)
	{
		uintptr_t valuePtr = reinterpret_cast<uintptr_t>(a_self);
		valuePtr += uintptr_t(m_offset);
		*a_outPtr = reinterpret_cast<void*>(valuePtr);

		// TODO: Return false if field isn't valid
		return true;
	}
}
