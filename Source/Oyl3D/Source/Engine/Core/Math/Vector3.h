#pragma once

#include "Internal/type_Vector3.h"

namespace Oyl
{
	using Vector3f = Vector_t<3, float>;
	static_assert(sizeof(Vector3f) == sizeof(Vector3f::data));

	using Vector3 = Vector3f;

	using Vector3i = Vector_t<3, int>;
	static_assert(sizeof(Vector3i) == sizeof(Vector3i::data));

	using Vector3u = Vector_t<3, unsigned int>;
	static_assert(sizeof(Vector3u) == sizeof(Vector3u::data));
}