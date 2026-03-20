#pragma once

#include <stdint.h>

namespace Spyll::Reflection
{
	namespace Detail
	{
		using TypeIdUnderlying = uint32_t;
	}

	enum class TypeId : Detail::TypeIdUnderlying { Null = -1 };

	constexpr
	bool
	operator ==(TypeId a_lhs, TypeId a_rhs)
	{
		auto lhs = static_cast<Detail::TypeIdUnderlying>(a_lhs);
		auto rhs = static_cast<Detail::TypeIdUnderlying>(a_rhs);

		return lhs == rhs;
	}

	constexpr
	bool
	operator !=(TypeId a_lhs, TypeId a_rhs)
	{
		return !(a_lhs == a_rhs);
	}
}
