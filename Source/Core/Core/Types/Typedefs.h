#pragma once

namespace Oyl
{
	using i8  = int8_t;
	using i16 = int16_t;
	using i32 = int32_t;
	using i64 = int64_t;
	
	using u8  = uint8_t;
	using u16 = uint16_t;
	using u32 = uint32_t;
	using u64 = uint64_t;

	using uint = u32;

	using f32 = float_t;
	static_assert(sizeof(f32) == 4);
	using f64 = double_t;
	static_assert(sizeof(f64) == 8);
}

#if OYL_SIMPLE_NUMERIC_TYPEDEFS
using Oyl::i8;
using Oyl::i16;
using Oyl::i32;
using Oyl::i64;

using Oyl::u8 ;
using Oyl::u16;
using Oyl::u32;
using Oyl::u64;

using Oyl::uint;

using Oyl::f32;
using Oyl::f64;
#endif