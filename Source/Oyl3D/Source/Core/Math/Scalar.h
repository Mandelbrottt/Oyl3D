#pragma once

#include <type_traits>
#include <cmath>

#include "Constants.h"

namespace Oyl::Math
{
	/**
	 * \brief Returns the sign of the given value
	 * \return -1 if a_value < 0,
	 *          0 if a_value == 0,
	 *          1 if a_value > 0
	 */
	template<typename TScalar>
	constexpr
	TScalar
	Sign(TScalar a_value)
	{
		if (a_value < 0)
		{
			return -1;
		}

		if (a_value > 0)
		{
			return 1;
		}
		
		return 0;
	}

	template<typename TScalar>
	constexpr
	TScalar
	Square(TScalar a_value)
	{
		return a_value * a_value;
	}
	
	template<typename TScalar, std::enable_if_t<std::is_floating_point_v<TScalar>, bool> = true>
	constexpr
	TScalar
	Lerp(TScalar a_t, TScalar a_lhs, TScalar a_rhs)
	{
		return a_lhs * (1 - a_t) + a_rhs * a_t;
	}

	/** 
	 * \return The angle about the origin of the vector with y component a_y and x component a_x.
	 *         Takes into account the quadrant of the vector.
	 */
	template<typename TScalar>
	constexpr
	TScalar
	Atan2(TScalar a_y, TScalar a_x)
	{
		// Flip x and y for some reason
		// Probably a side effect of how we calculate angles in the model matrix
		return std::atan2(a_x, a_y) * RAD_TO_DEG;
	}
	
	/** 
	 * \return The arc-sin of the ratio a_x
	 */
	template<typename TScalar>
	constexpr
	TScalar
	Asin(TScalar a_x)
	{
		return std::asin(a_x) * RAD_TO_DEG;
	}
}
