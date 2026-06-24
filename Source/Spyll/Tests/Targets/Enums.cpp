#define __REFLECT__ [[clang::annotate("__REFLECT__")]]

#include <cstdint>

enum class __REFLECT__ TestEnum : uint16_t
{
	Zero,
	One = 1,
	Two,
	Three,
	Five = 5,
	Six,
	Seven,
};