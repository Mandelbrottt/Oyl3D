#pragma once

#include <type_traits>

namespace Oyl::Traits
{
#pragma region Base

	template<typename T>
	struct AddConst
	{
		using type = std::add_const_t<T>;
	};

	template<typename T>
	using AddConst_T = typename AddConst<T>::type;

	template<typename T>
	struct RemoveConst
	{
		using type = std::remove_const_t<T>;
	};

	template<typename T>
	using RemoveConst_T = typename RemoveConst<T>::type;

#pragma endregion
#pragma region Pointers

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

#pragma endregion
#pragma region Function Pointers

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

	template<typename>
	struct FunctionSignatureFromPointer;

	template<typename T>
	using FunctionSignatureFromPointer_T = typename FunctionSignatureFromPointer<T>::type;

	template<typename TReturn, typename... TArgs>
	struct FunctionSignatureFromPointer<TReturn(TArgs...)>
	{
		using type = TReturn(TArgs...);
	};

	template<typename TReturn, typename... TArgs>
	struct FunctionSignatureFromPointer<TReturn(*)(TArgs...)>
	{
		using type = TReturn(TArgs...);
	};

#pragma endregion
#pragma region Member Function Pointers

	template<typename>
	struct IsMemberFunctionPointer : std::false_type {};

	#define _IS_MEMBER_FUNCTION_POINTER(_cv_ref_noexcept_) \
		template<typename TClass, typename TReturn, typename... TArgs> \
		struct IsMemberFunctionPointer<TReturn (TClass::*)(TArgs...) _cv_ref_noexcept_> : std::true_type {};

	_CLASS_DEFINE_CV_REF_NOEXCEPT(_IS_MEMBER_FUNCTION_POINTER)
	#undef _IS_MEMBER_FUNCTION_POINTER

	template<typename T>
	concept MemberFunctionPointer = IsMemberFunctionPointer<T>::value;

	#define _FUNCTION_SIGNATURE_FROM_POINTER(_cv_ref_noexcept_) \
		template<typename TClass, typename TReturn, typename... TArgs> \
		struct FunctionSignatureFromPointer<TReturn(TClass::*)(TArgs...) _cv_ref_noexcept_> \
		{ \
			using type = TReturn(TArgs...); \
		};

	_CLASS_DEFINE_CV_REF_NOEXCEPT(_FUNCTION_SIGNATURE_FROM_POINTER)
	#undef _FUNCTION_SIGNATURE_FROM_POINTER

	template<MemberFunctionPointer>
	struct AddMemberFunctionConst;

	template<typename TClass, typename TReturn, typename... TArgs>
	struct AddMemberFunctionConst<TReturn(TClass::*)(TArgs...)>
	{
		using type = TReturn(TClass::*)(TArgs...) const;
	};

	template<typename TClass, typename TReturn, typename... TArgs>
	struct AddMemberFunctionConst<TReturn(TClass::*)(TArgs...) const>
	{
		using type = TReturn(TClass::*)(TArgs...) const;
	};

	template<typename TClass, typename TReturn, typename... TArgs>
	struct AddMemberFunctionConst<TReturn(TClass::*)(TArgs...) noexcept>
	{
		using type = TReturn(TClass::*)(TArgs...) const noexcept;
	};

	template<typename TClass, typename TReturn, typename... TArgs>
	struct AddMemberFunctionConst<TReturn(TClass::*)(TArgs...) const noexcept>
	{
		using type = TReturn(TClass::*)(TArgs...) const noexcept;
	};

	template<MemberFunctionPointer>
	struct RemoveMemberFunctionConst;

	template<typename TClass, typename TReturn, typename... TArgs>
	struct RemoveMemberFunctionConst<TReturn(TClass::*)(TArgs...)>
	{
		using type = TReturn(TClass::*)(TArgs...);
	};

	template<typename TClass, typename TReturn, typename... TArgs>
	struct RemoveMemberFunctionConst<TReturn(TClass::*)(TArgs...) const>
	{
		using type = TReturn(TClass::*)(TArgs...);
	};

	template<typename TClass, typename TReturn, typename... TArgs>
	struct RemoveMemberFunctionConst<TReturn(TClass::*)(TArgs...) noexcept>
	{
		using type = TReturn(TClass::*)(TArgs...) noexcept;
	};

	template<typename TClass, typename TReturn, typename... TArgs>
	struct RemoveMemberFunctionConst<TReturn(TClass::*)(TArgs...) const noexcept>
	{
		using type = TReturn(TClass::*)(TArgs...) noexcept;
	};

	template<typename>
	struct MemberFunctionClass;

	template<typename Class, typename TReturn, typename... TArgs>
	struct MemberFunctionClass<TReturn(Class::*)(TArgs...)>
	{
		using type = Class;
	};

	template<typename Class, typename TReturn, typename... TArgs>
	struct MemberFunctionClass<TReturn(Class::*)(TArgs...) const>
	{
		using type = Class;
	};

	template<typename Class, typename TReturn, typename... TArgs>
	struct MemberFunctionClass<TReturn(Class::*)(TArgs...) noexcept>
	{
		using type = Class;
	};

	template<typename Class, typename TReturn, typename... TArgs>
	struct MemberFunctionClass<TReturn(Class::*)(TArgs...) const noexcept>
	{
		using type = Class;
	};

	template<typename T>
	using MemberFunctionBaseClass_T = typename MemberFunctionClass<T>::type;

	template<typename TClass, typename>
	struct MemberFunctionWithSignature;

	template<typename TClass, typename TReturn, typename... TArgs>
	struct MemberFunctionWithSignature<TClass, TReturn(TArgs...)>
	{
		using type = TReturn(TClass::*)(TArgs...);
	};

	template<typename TClass, typename TReturn, typename... TArgs>
	using MemberFunctionWithSignature_T = typename MemberFunctionWithSignature<TClass, TReturn(TArgs...)>::type;

#pragma endregion Member Functions

	template<typename T>
	struct UnderlyingType
	{
		using type = std::underlying_type_t<T>;
	};

	template<typename T>
	using UnderlyingType_T = typename UnderlyingType<T>::type;
}
