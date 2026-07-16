#pragma once

#define OYL_SIMPLE_NUMERIC_TYPEDEFS
#if defined(OYL_SIMPLE_NUMERIC_TYPEDEFS)
	#define _OYL_SIMPLE_NUMERIC_TYPEDEFS_START inline namespace Primitives {
	#define _OYL_SIMPLE_NUMERIC_TYPEDEFS_END   }
	#define _OYL_SIMPLE_NUMERIC_TYPEDEFS_USING using namespace Oyl::Primitives;
#else
	#define _OYL_SIMPLE_NUMERIC_TYPEDEFS_START
	#define _OYL_SIMPLE_NUMERIC_TYPEDEFS_END
	#define _OYL_SIMPLE_NUMERIC_TYPEDEFS_USING
#endif

namespace Oyl
{
	_OYL_SIMPLE_NUMERIC_TYPEDEFS_START
		using int8 = int8_t;
		using int16 = int16_t;
		using int32 = int32_t;
		using int64 = int64_t;

		using uint8 = uint8_t;
		using uint16 = uint16_t;
		using uint32 = uint32_t;
		using uint64 = uint64_t;

		using uint = uint32;
		using byte = uint8;
		using cbyte = const byte;

		using float32 = float_t;
		static_assert(sizeof(float32) == 4);
		using float64 = double_t;
		static_assert(sizeof(float64) == 8);
	_OYL_SIMPLE_NUMERIC_TYPEDEFS_END
}

_OYL_SIMPLE_NUMERIC_TYPEDEFS_USING

#if defined(OYL_SIMPLE_NUMERIC_TYPEDEFS)
	#undef _OYL_SIMPLE_NUMERIC_TYPEDEFS_START
	#undef _OYL_SIMPLE_NUMERIC_TYPEDEFS_END
	#undef _OYL_SIMPLE_NUMERIC_TYPEDEFS_USING
#endif
