#define __REFLECT__ [[clang::annotate("reflect")]]

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