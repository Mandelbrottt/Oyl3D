#pragma once

#include "Internal/type_Matrix3.h"

namespace Oyl
{
	using Matrix3 = Matrix_t<3, 3, float>;
	static_assert(sizeof(Matrix3) == sizeof(Matrix3::data));
}