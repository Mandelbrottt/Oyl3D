#pragma once

#include "Detail/type_Matrix3.h"

namespace Oyl
{
	typedef Matrix_t<3, 3, float> Matrix3;
	static_assert(sizeof(Matrix3) == sizeof(Matrix3::data));
}