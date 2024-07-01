#pragma once

namespace Oyl
{
	using int8  = int8_t;
	using int16 = int16_t;
	using int32 = int32_t;
	using int64 = int64_t;
	
	using uint8  = uint8_t;
	using uint16 = uint16_t;
	using uint32 = uint32_t;
	using uint64 = uint64_t;

	using uint = uint32;

	using float32 = float_t;
	static_assert(sizeof(float32) == 4);
	using float64 = double_t;
	static_assert(sizeof(float64) == 8);
}

#if OYL_SIMPLE_NUMERIC_TYPEDEFS
using Oyl::int8;
using Oyl::int16;
using Oyl::int32;
using Oyl::int64;

using Oyl::uint8;
using Oyl::uint16;
using Oyl::uint32;
using Oyl::uint64;

using Oyl::uint;

using Oyl::float32;
using Oyl::float64;
#endif