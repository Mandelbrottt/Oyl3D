#pragma once

#include <type_traits>

namespace Oyl::Traits
{
#pragma region Primitive

	template<bool Value>
	struct BoolConstant
	{
		static constexpr bool value = Value;
	};

	struct TrueType : BoolConstant<true> {};

	struct FalseType : BoolConstant<false> {};

	template<typename T>
	concept Integer = std::is_integral_v<T>;

	template<typename T>
	concept SignedInteger = Integer<T> && std::is_signed_v<T>;

	template<typename T>
	concept UnsignedInteger = Integer<T> && std::is_unsigned_v<T>;

	template<typename T>
	struct IsEnum : std::is_enum<T> {};

	template<typename T>
	concept Enum = IsEnum<T>::value;

	template<typename T>
	struct TypeConstant
	{
		using type = T;
	};

#pragma endregion Primitive
#pragma region Base

	template<typename T>
	struct AddConst : TypeConstant<std::add_const_t<T>> {};

	template<typename T>
	using TAddConst = typename AddConst<T>::type;

	template<typename T>
	struct RemoveConst : TypeConstant<std::remove_const_t<T>> {};

	template<typename T>
	using TRemoveConst = typename RemoveConst<T>::type;

	template<typename TFrom, typename TTo>
	struct IsImplicitlyConvertibleTo : BoolConstant<std::is_convertible_v<TFrom, TTo>> {};

	template<typename TFrom, typename TTo>
	concept ImplicitlyConvertibleTo = IsImplicitlyConvertibleTo<TFrom, TTo>::value;

	namespace Internal
	{
		template<typename TFrom, typename TTo>
		auto TestExplicitlyConvertible(int) -> decltype(static_cast<TTo>(std::declval<TFrom>()), TrueType {});

		template<typename TFrom, typename TTo>
		auto TestExplicitlyConvertible(...) -> FalseType;
	}

	template<typename TFrom, typename TTo>
	struct IsExplicitlyConvertibleTo : BoolConstant<decltype(Internal::TestExplicitlyConvertible<TFrom, TTo>(0))::value> {};

	template<typename TFrom, typename TTo>
	concept ExplicitlyConvertibleTo = IsExplicitlyConvertibleTo<TFrom, TTo>::value;

	template<typename TFrom, typename TTo>
	struct IsConvertibleTo
		: BoolConstant<ImplicitlyConvertibleTo<TFrom, TTo>
		               && ExplicitlyConvertibleTo<TFrom, TTo>> {};

	template<typename TFrom, typename TTo>
	concept ConvertibleTo = IsConvertibleTo<TFrom, TTo>::value;

#pragma endregion Base
#pragma region Pointers

	template<typename T>
	struct IsPointer : BoolConstant<std::is_pointer_v<T>> {};

	template<typename T>
	concept Pointer = IsPointer<T>::value;

	template<typename T>
	struct AddPointer : TypeConstant<std::add_pointer_t<T>> {};

	template<typename T>
	using TAddPointer = typename AddPointer<T>::type;

	template<typename T>
	struct RemovePointer : TypeConstant<std::remove_pointer_t<T>> {};

	template<typename T>
	using RemovePointer_T = typename RemovePointer<T>::type;

	template<typename T>
	struct IsVoidPointer : BoolConstant<std::is_void_v<RemovePointer<T>>> {};

	template<typename T>
	concept VoidPointer = IsVoidPointer<T>::value;

	template<typename T>
	concept NonVoidPointer = !IsVoidPointer<T>::value;

	template<typename T>
	concept PointerToObject = IsPointer<T>::value && !IsPointer<RemovePointer_T<T>>::value;

	template<typename T>
	struct IsPointerToPointer : BoolConstant<Pointer<T> && Pointer<RemovePointer_T<T>>> {};

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
	using TMemberFunctionWithSignature = typename MemberFunctionWithSignature<TClass, TReturn(TArgs...)>::type;

#pragma endregion Member Functions

	template<typename T>
	struct UnderlyingType
	{
		using type = std::underlying_type_t<T>;
	};

	template<typename T>
	using TUnderlyingType = typename UnderlyingType<T>::type;
}
