#pragma once

#include "Internal/type_Matrix4.h"

namespace Oyl
{
	using Matrix4 = Matrix_t<4, 4, float>;
	static_assert(sizeof(Matrix4) == sizeof(Matrix4::data));
}