#pragma once
#pragma once

#pragma region Platform
#	if defined(_WIN32)
#		define OYL_WINDOWS 1
#		if defined(_WIN64)
#			define OYL_WIN64 1
#		else
#			define OYL_WIN32 1
#		endif
#
#		define NOMINMAX
#	else
#
#	endif
#pragma endregion

#pragma region ABI
#	if defined(_MSC_VER) && !(defined(__MINGW32__) || defined(__MINGW64__))
#		define __OYL__ABI_MSC
#	elif defined(__clang__) || defined(__GNUC__)
#		define __OYL__ABI_ITANIUM
#	endif
#pragma endregion

#pragma region Shared Library Export
#	if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
#		define __OYL__EXPORT_ATTR __declspec(dllexport)
#		define __OYL__IMPORT_ATTR __declspec(dllimport)
#	elif defined(__clang__) || defined(__GNUC__)
#		define __OYL__EXPORT_ATTR __attribute__((visibility("default")))
#		define __OYL__IMPORT_ATTR __attribute__((visibility("default")))
#	else
#		error Unsupported platform!
#	endif
#pragma endregion

#pragma region Attributes
#	define OYL_DEPRECATED(_message_) [[deprecated(_message_)]]
#	define OYL_UNUSED(...) ([](auto&&...){})(__VA_ARGS__)
#pragma endregion

#pragma region Construction Macros
#	if defined(DEFAULT_COPY)
#		undef DEFAULT_COPY
#	endif
#	define DEFAULT_COPY(_class_) \
		_class_(const _class_&) = default; \
		_class_& \
		operator =(const _class_&) = default

#	if defined(DEFAULT_MOVE)
#		undef DEFAULT_MOVE
#	endif
#	define DEFAULT_MOVE(_class_) \
		_class_(_class_&&) noexcept = default; \
		_class_& \
		operator =(_class_&&) noexcept = default

#	if defined(NO_COPY)
#		undef NO_COPY
#	endif
#	define NO_COPY(_class_) \
		_class_(const _class_&) = delete; \
		_class_& \
		operator =(const _class_&) = delete

#	if defined(NO_MOVE)
#		undef NO_MOVE
#	endif
#	define NO_MOVE(_class_) \
		_class_(_class_&&) noexcept = delete; \
		_class_& \
		operator =(_class_&&) noexcept = delete
#pragma endregion Construction Macros

#pragma region Macro Utils
#	define OYL_EXPAND(_x_) _x_
#	define OYL_STRINGIFY(_x_) #_x_
#	define OYL_STRINGIFY_MACRO(_x_) OYL_STRINGIFY(_x_)

#	define OYL_FORCE_SEMICOLON static_assert(true)
#	define OYL_FORCE_FORMAT_INDENT static_assert(true);

#	define OYL_CAT(_a_, _b_) _a_##_b_
#	define OYL_CAT_EXPAND(_a_, _b_) OYL_CAT(_a_, _b_)
#	define OYL_CAT_WITH_UNDERSCORE(_name_, _num_) OYL_CAT(_name_##_, _num_)
#pragma endregion

#pragma region Debug Macros
#	if !defined(OYL_DISTRIBUTION)
#		if defined(_MSC_VER)
#			define OYL_BREAKPOINT ::__debugbreak()
#		else
#			warning "Breakpoints only implemented for MSVC"
#			define OYL_BREAKPOINT
#		endif
#		define OYL_STRIP_IN_DISTRIBUTION(...) __VA_ARGS__
#	else
#		define OYL_BREAKPOINT
#		define OYL_STRIP_IN_DISTRIBUTION(...)
#	endif
#pragma endregion

#pragma region Macro Argument Overloading
/**
 * Overload a macro, and allow for one macro definition to map to multiple different macros, depending on
 * argument count
 *
 * \param _name_ The name of the defined overloaded macros minus the underscore and number at the end
 *
 * <code>
 * #define OVERLOADED_MACRO(...) OYL_MACRO_OVERLOAD(_OVERLOADED_MACRO, __VA_ARGS__)
 *
 * #define _OVERLOADED_MACRO_1() // 1 arguments
 * #define _OVERLOADED_MACRO_2() // 2 arguments
 * #define _OVERLOADED_MACRO_3() // 3 arguments
 *
 * and so on...
 *
 * // You can omit any number of macro overload definitions if you only want to support specific amounts
 * </code>
 */
#	define OYL_MACRO_OVERLOAD(_name_, ...) OYL_EXPAND(_OYL_MACRO_APPEND_ARG_COUNT(_name_, __VA_ARGS__))

#	define OYL_GET_ARG_COUNT(_1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, _9_, _10_, _11_, _12_, _13_, _14_, _15_, _16_, _17_, _18_, _19_, _20_, _count_, ...) _count_

#	define OYL_EXPAND_ARGS_COUNT(...)              OYL_EXPAND(OYL_GET_ARG_COUNT(__VA_ARGS__, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1))
#	define OYL_MACRO_APPEND_ARG_COUNT(_name_, ...) OYL_EXPAND(OYL_CAT_WITH_UNDERSCORE(_name_, OYL_EXPAND_ARGS_COUNT(unused, __VA_ARGS__))(__VA_ARGS__))
#pragma endregion
