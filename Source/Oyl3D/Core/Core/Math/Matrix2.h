#pragma once

#include "Detail/type_Matrix2.h"

namespace Oyl
{
	typedef Matrix_t<2, 2, float> Matrix2;
	static_assert(sizeof(Matrix2) == sizeof(Matrix2::data));
}