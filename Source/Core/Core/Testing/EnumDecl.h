#pragma once
//
//#include "Core/Common.h"
//
//#define ELEMENT_OF_LIST(_x_) _x_,
//#define ELEMENT_OF_LIST_STRINGIFY(_x_) _OYL_EXPAND(ELEMENT_OF_LIST(_OYL_STRINGIFY(_x_)))
//#define ELEMENT_OF_LIST_NAME_TO_STRING(_x_) if (a_value == _ENUM_::_x_) return #_x_;
//
//#define FOR_EACH_ARG(...) OYL_MACRO_OVERLOAD(_FOR_EACH_ARG, __VA_ARGS__)
//#define _FOR_EACH_ARG_1(_apply_macro_)
//#define _FOR_EACH_ARG_2(_apply_macro_, _1_) _OYL_EXPAND(_apply_macro_(_1_))
//#define _FOR_EACH_ARG_3(_apply_macro_, _1_, ...) _OYL_EXPAND(_apply_macro_(_1_)) _OYL_EXPAND(_FOR_EACH_ARG_2(_apply_macro_, __VA_ARGS__))
//#define _FOR_EACH_ARG_4(_apply_macro_, _1_, ...) _OYL_EXPAND(_apply_macro_(_1_)) _OYL_EXPAND(_FOR_EACH_ARG_3(_apply_macro_, __VA_ARGS__))
//#define _FOR_EACH_ARG_5(_apply_macro_, _1_, ...) _OYL_EXPAND(_apply_macro_(_1_)) _OYL_EXPAND(_FOR_EACH_ARG_4(_apply_macro_, __VA_ARGS__))
//#define _FOR_EACH_ARG_6(_apply_macro_, _1_, ...) _OYL_EXPAND(_apply_macro_(_1_)) _OYL_EXPAND(_FOR_EACH_ARG_5(_apply_macro_, __VA_ARGS__))
//#define _FOR_EACH_ARG_7(_apply_macro_, _1_, ...) _OYL_EXPAND(_apply_macro_(_1_)) _OYL_EXPAND(_FOR_EACH_ARG_6(_apply_macro_, __VA_ARGS__))
//#define _FOR_EACH_ARG_8(_apply_macro_, _1_, ...) _OYL_EXPAND(_apply_macro_(_1_)) _OYL_EXPAND(_FOR_EACH_ARG_7(_apply_macro_, __VA_ARGS__))
//
//#define ENUM_DECL(_name_, ...) \
//	enum class _name_ { \
//		FOR_EACH_ARG(ELEMENT_OF_LIST, __VA_ARGS__)\
//	}; \
//	\
//	std::string \
//	ToString(_name_ a_value) \
//	{ \
//		using _ENUM_ = _name_;\
//		FOR_EACH_ARG(ELEMENT_OF_LIST_NAME_TO_STRING, __VA_ARGS__) \
//		return "";\
//	}
//
//ENUM_DECL(
//	Name,
//	Value1,
//	Value2,
//	Value3
//);