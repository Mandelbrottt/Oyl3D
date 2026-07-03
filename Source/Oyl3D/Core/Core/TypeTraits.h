#pragma once

#include <type_traits>

namespace Oyl::Traits
{
	template<typename T>
	struct IsVoidPointer
	{
		static constexpr bool value = std::is_void_v<std::remove_pointer_t<T>>;
	};

	template<typename T>
	concept VoidPointer = IsVoidPointer<T>::value;

	template<typename T>
	concept NonVoidPointer = !IsVoidPointer<T>::value;

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
}
