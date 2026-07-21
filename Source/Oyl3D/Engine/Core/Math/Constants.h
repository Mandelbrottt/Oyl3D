#pragma once

#if defined(PI)
	#undef PI
#endif

namespace Oyl::Math
{
	constexpr float PI = 3.1415f;

	constexpr float DEG_TO_RAD = PI / 180.f;
	constexpr float RAD_TO_DEG = 180.f / PI;
}