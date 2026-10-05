#pragma once

#include "Internal/type_Matrix2.h"

namespace Oyl
{
	using Matrix2 = Matrix_t<2, 2, float>;
	static_assert(sizeof(Matrix2) == sizeof(Matrix2::data));
}