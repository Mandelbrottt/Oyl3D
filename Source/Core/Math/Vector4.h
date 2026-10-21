#pragma once

#include "Detail/type_Vector4.h"

namespace Oyl
{
	typedef Vector_t<4, float> Vector4f;
	static_assert(sizeof(Vector4f) == sizeof(Vector4f::data));

	typedef Vector4f Vector4;
	
	typedef Vector_t<4, int> Vector4i;
	static_assert(sizeof(Vector4i) == sizeof(Vector4i::data));
	
	typedef Vector_t<4, unsigned int> Vector4u;
	static_assert(sizeof(Vector4u) == sizeof(Vector4u::data));
}