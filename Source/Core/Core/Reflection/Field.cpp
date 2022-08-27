#include "pch.h"

#include "Type.h"
#include "Field.h"

namespace Rearm::Reflection
{
	const Type&
	Field::FieldType() const
	{
		const auto* result = Type::TryGet(m_fieldTypeId);
		assert(result != nullptr);
		return *result;
	}

	const Type&
	Field::ContainingType() const
	{
		const auto* result = Type::TryGet(m_containingTypeId);
		assert(result != nullptr);
		return *result;
	}
}
