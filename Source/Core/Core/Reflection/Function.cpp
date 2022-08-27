#include "pch.h"

#include "Function.h"

namespace Rearm::Reflection
{
	const Type&
	Function::ContainingType() const
	{
		const auto* result = Type::TryGet(m_containingTypeId);
		assert(result != nullptr);
		return *result;
	}

	const Type&
	Function::ReturnType() const
	{
		const auto* result = Type::TryGet(m_returnTypeId);
		assert(result != nullptr);
		return *result;
	}

	std::vector<const Type*>
	Function::ParameterTypes() const
	{
		std::vector<const Type*> result;
		result.reserve(m_parameterTypeIds.size());

		for (int i = 0; i < m_parameterTypeIds.size(); i++)
		{
			const auto* paramType = Type::TryGet(m_parameterTypeIds[i]);
			assert(paramType != nullptr);
			result.emplace_back(paramType);
		}

		return result;
	}
}
