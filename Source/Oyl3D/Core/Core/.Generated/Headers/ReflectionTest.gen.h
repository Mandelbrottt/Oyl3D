#pragma once

#ifdef GeneratedBody
#undef GeneratedBody
#endif

#define GeneratedBody() \
	static_assert(true); \
private: \
	using This = Something;\
	/*friend is_member_visible<Something>;*/ \
public: \
	static \
	std::string_view \
	Name() { return "Something"; } \
	\
	std::string_view \
	GetName() const override { return This::Name(); }

#include <type_traits>

template<typename, typename = void>
struct is_member_visible : std::false_type {};

template<typename T>
struct is_member_visible<T, std::void_t<decltype(int { std::declval<T>().member })>> : std::true_type {};

template<typename T>
constexpr bool is_member_visible_v = is_member_visible<T>::value;