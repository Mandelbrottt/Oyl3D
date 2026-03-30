#include "Field.h"

#include "Type.h"

namespace Spyll::Reflection
{
	struct Field::Impl
	{
		std::string name;
		std::string qualifiedName;

		Type* type = nullptr;

		Type* ownerType = nullptr;
		uint32_t offset;

		AccessSpecifier accessSpecifier = AccessSpecifier::None;

		bool isConst = false;
		bool isVolatile = false;
		bool isReference = false;
		bool isPointer = false;
	};
	
	Field::Field()
	{
		m_impl = new Impl;
	}

	Field::~Field()
	{
		delete m_impl;
		m_impl = nullptr;
	}

	std::string_view
	Field::GetName() const
	{
		return m_impl->name;
	}

	std::string_view
	Field::GetQualifiedName() const
	{
		return m_impl->qualifiedName;
	}

	const Type*
	Field::GetType() const
	{
		return m_impl->type;
	}

	const Type*
	Field::GetOwningType() const
	{
		return m_impl->ownerType;
	}

	uint32_t
	Field::GetOffset() const
	{
		return m_impl->offset;
	}

	AccessSpecifier
	Field::GetAccessSpecifier() const
	{
		return m_impl->accessSpecifier;
	}

	bool
	Field::IsConst() const
	{
		return m_impl->isConst;
	}

	bool
	Field::IsVolatile() const
	{
		return m_impl->isVolatile;
	}

	bool
	Field::IsPointer() const
	{
		return m_impl->isPointer;
	}

	bool
	Field::IsReference() const
	{
		return m_impl->isReference;
	}

	bool
	Field::GetValue(void* a_self, void** a_outPtr)
	{
		uintptr_t valuePtr = reinterpret_cast<uintptr_t>(a_self);
		valuePtr += uintptr_t(m_impl->offset);
		*a_outPtr = reinterpret_cast<void*>(valuePtr);

		// TODO: Return false if field isn't valid
		return true;
	}
}
