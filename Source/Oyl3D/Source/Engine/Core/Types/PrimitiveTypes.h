#pragma once

#include <cstdint>

namespace Oyl
{
	using int8 = int8_t;
	using int16 = int16_t;
	using int32 = int32_t;
	using int64 = int64_t;
	static_assert(sizeof(int8) == 1);
	static_assert(sizeof(int16) == 2);
	static_assert(sizeof(int32) == 4);
	static_assert(sizeof(int64) == 8);

	using uint8 = uint8_t;
	using uint16 = uint16_t;
	using uint32 = uint32_t;
	using uint64 = uint64_t;
	static_assert(sizeof(uint8) == 1);
	static_assert(sizeof(uint16) == 2);
	static_assert(sizeof(uint32) == 4);
	static_assert(sizeof(uint64) == 8);

	using uint = unsigned int;
	using byte = uint8;
	static_assert(sizeof(byte) == 1);

	using float32 = float;
	using float64 = double;
	static_assert(sizeof(float32) == 4);
	static_assert(sizeof(float64) == 8);
}

#if !defined(OYL_QUALIFIED_NUMERIC_TYPEDEFS)
using Oyl::int8;
using Oyl::int16;
using Oyl::int32;
using Oyl::int64;

using Oyl::uint8;
using Oyl::uint16;
using Oyl::uint32;
using Oyl::uint64;

using Oyl::uint;
using Oyl::byte;

using Oyl::float32;
using Oyl::float64;
#endif