#pragma once

#if defined(__REFLECT_GENERATE__)
	#define OYL_REFLECT __attribute__((annotate("__REFLECT__")))
#else
	#define OYL_REFLECT
#endif
