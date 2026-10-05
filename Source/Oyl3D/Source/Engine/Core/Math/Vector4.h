#pragma once

#include "Internal/type_Vector4.h"

namespace Oyl
{
	using Vector4f = Vector_t<4, float>;
	static_assert(sizeof(Vector4f) == sizeof(Vector4f::data));

	using Vector4 = Vector4f;

	using Vector4i = Vector_t<4, int>;
	static_assert(sizeof(Vector4i) == sizeof(Vector4i::data));

	using Vector4u = Vector_t<4, unsigned int>;
	static_assert(sizeof(Vector4u) == sizeof(Vector4u::data));
}
