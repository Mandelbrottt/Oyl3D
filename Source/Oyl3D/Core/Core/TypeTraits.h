#pragma once

#include <type_traits>

namespace Oyl::Traits
{
	template<typename T>
	struct IsPointer
	{
		static constexpr bool value = std::is_pointer_v<T>;
	};

	template<typename T>
	concept Pointer = IsPointer<T>::value;

	template<typename T>
	struct RemovePointer
	{
		using type = std::remove_pointer_t<T>;
	};

	template<typename T>
	using RemovePointer_T = typename RemovePointer<T>::type;

	template<typename T>
	struct IsVoidPointer
	{
		static constexpr bool value = std::is_void_v<RemovePointer<T>>;
	};

	template<typename T>
	concept VoidPointer = IsVoidPointer<T>::value;

	template<typename T>
	concept NonVoidPointer = !IsVoidPointer<T>::value;

	template<typename T>
	concept PointerToObject = IsPointer<T>::value && !IsPointer<RemovePointer_T<T>>::value;

	template<typename T>
	struct IsPointerToPointer
	{
		static constexpr bool value = IsPointer<T>::value && IsPointer<RemovePointer_T<T>>::value;
	};

	template<typename T>
	concept PointerToPointer = IsPointerToPointer<T>::value;

	template<typename T>
	struct IsFunctionPointer
	{
		static constexpr bool value = std::is_pointer_v<T> && std::is_function_v<std::remove_pointer_t<T>>;
	};

	template<typename T>
	concept FunctionPointer = IsFunctionPointer<T>::value;

	template<typename>
	struct IsFunctionSignature : std::false_type {};

	template<typename TRet, typename... TArgs>
	struct IsFunctionSignature<TRet (TArgs...)> : std::true_type {};

	template<typename T>
	concept FunctionSignature = IsFunctionSignature<T>::value;

	template<typename T>
	concept FunctionObject = FunctionPointer<T> || FunctionSignature<T>;

	template<typename T>
	struct RemoveMemberFunctionConst
	{
		using type = T;
	};

	template<typename TObj, typename TReturn, typename... TArgs>
	struct RemoveMemberFunctionConst<TReturn(TObj::*)(TArgs...)>
	{
		using type = TReturn(TObj::*)(TArgs...);
	};

	template<typename TObj, typename TReturn, typename... TArgs>
	struct RemoveMemberFunctionConst<TReturn(TObj::*)(TArgs...) const>
	{
		using type = TReturn(TObj::*)(TArgs...);
	};

	template<typename T>
	using RemoveMemberFunctionConst_T = typename RemoveMemberFunctionConst<T>::type;

	template<typename T>
	struct UnderlyingType
	{
		using type = std::underlying_type_t<T>;
	};

	template<typename T>
	using UnderlyingType_T = typename UnderlyingType<T>::type;
}
