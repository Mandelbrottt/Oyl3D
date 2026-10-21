#pragma once

#include "Internal/type_Vector2.h"

namespace Oyl
{
	using Vector2f = Vector_t<2, float>;
	static_assert(sizeof(Vector2f) == sizeof(Vector2f::data));

	using Vector2 = Vector2f;

	using Vector2i = Vector_t<2, int>;
	static_assert(sizeof(Vector2i) == sizeof(Vector2i::data));

	using Vector2u = Vector_t<2, unsigned int>;
	static_assert(sizeof(Vector2u) == sizeof(Vector2u::data));
}
