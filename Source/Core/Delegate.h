#pragma once

#include "TypeTraits.h"

namespace Oyl
{
	template<Traits::FunctionSignature>
	class Delegate;

	template<typename TReturn, typename... TArgs>
	class Delegate<TReturn(TArgs...)>
	{
	public:
		using Fn = TReturn(TArgs...);

		template<auto Function>
			requires Traits::MemberFunctionPointer<decltype(Function)>
		using MemberFnFromValue = TReturn (Traits::MemberFunctionBaseClass_T<decltype(Function)>::*)(TArgs...);

		template<typename TClass>
		using MemberFnFromType = TReturn (TClass::*)(TArgs...);

		Delegate() {}

		Delegate(std::nullptr_t)
			: Delegate() {}

		Delegate(const Delegate& a_other)
		{
			*this = a_other;
		}

		Delegate&
		operator=(const Delegate& a_other)
		{
			m_obj = a_other.m_obj;
			m_fn = a_other.m_fn;

			return *this;
		}

		Delegate(Delegate&& a_other) noexcept
		{
			*this = std::move(a_other);
		}

		Delegate&
		operator =(Delegate&& a_other) noexcept
		{
			std::swap(m_obj, a_other.m_obj);
			std::swap(m_fn, a_other.m_fn);

			return *this;
		}

		friend bool operator ==(const Delegate& a_lhs, const Delegate& a_rhs)
		{
			return a_lhs.m_fn == a_rhs.m_fn && a_lhs.m_obj == a_rhs.m_obj;
		}

		friend bool operator !=(const Delegate& a_lhs, const Delegate& a_rhs)
		{
			return !(a_lhs == a_rhs);
		}

		bool
		IsValid() const
		{
			return m_fn != nullptr;
		}

		explicit
		operator bool() const
		{
			return IsValid();
		}

	#pragma region Function Pointers

		template<auto Function>
		explicit
		Delegate(decltype(Function))
			: Delegate()
		{
			Set<Function>();
		}

		static
		Delegate
		Create(Fn* a_fn)
		{
			Delegate result;
			result.Set(a_fn);
			return result;
		}

		void
		Set(Fn* a_fn)
		{
			m_obj = nullptr;
			m_fn = a_fn;
		}

		template<auto Function>
		void
		Set()
		{
			m_obj = nullptr;
			std::memcpy(&m_fn, &Function, sizeof(m_fn));
		}

	#pragma endregion Function Pointers
	#pragma region Member Functions

	#define _DELEGATE_MEMBER_CONSTRUCTOR(_cv_ref_noexcept_) \
		template<typename TObj, typename TClass> \
		Delegate(TObj* a_obj, TReturn(TClass::*a_fn)(TArgs...) _cv_ref_noexcept_) \
		{ \
			Set(a_obj, a_fn); \
		}

	_CLASS_DEFINE_CV_REF_NOEXCEPT(_DELEGATE_MEMBER_CONSTRUCTOR)
	#undef _DELEGATE_MEMBER_CONSTRUCTOR

	#define _DELEGATE_MEMBER_CREATE_FUNC(_cv_ref_noexcept_) \
		template<typename TObj, typename TClass> \
			requires std::is_base_of_v<TClass, TObj> \
		static \
		Delegate \
		Create(TObj* a_obj, TReturn(TClass::*a_fn)(TArgs...) _cv_ref_noexcept_) \
		{ \
			Delegate result; \
			result.Set(a_obj, a_fn); \
			return result; \
		}

	_CLASS_DEFINE_CV_REF_NOEXCEPT(_DELEGATE_MEMBER_CREATE_FUNC)
	#undef _DELEGATE_MEMBER_CREATE_FUNC

		template<auto Function>
			requires Traits::MemberFunctionPointer<decltype(Function)>
		static
		Delegate
		Create(Traits::MemberFunctionBaseClass_T<decltype(Function)>* a_obj)
		{
			Delegate result;
			result.Set(a_obj, Function);
			return result;
		}

	#define _DELEGATE_MEMBER_SET_FUNC(_cv_ref_noexcept_) \
		template<typename TObj, typename TClass> \
			requires std::is_base_of_v<TClass, TObj> \
		void \
		Set(TObj* a_obj, TReturn(TClass::*a_fn)(TArgs...) _cv_ref_noexcept_) \
		{ \
			m_obj = reinterpret_cast<void*>(a_obj); \
			std::memcpy(&m_fn, &a_fn, sizeof(m_fn)); \
		}

	_CLASS_DEFINE_CV_REF_NOEXCEPT(_DELEGATE_MEMBER_SET_FUNC)
	#undef _DELEGATE_MEMBER_SET_FUNC

		template<auto Function>
			requires Traits::MemberFunctionPointer<decltype(Function)>
		void
		Set(Traits::MemberFunctionBaseClass_T<decltype(Function)>* a_obj)
		{
			m_obj = reinterpret_cast<void*>(a_obj);
			std::memcpy(&m_fn, Function, sizeof(m_fn));
		}

	#pragma endregion Member Functions
	#pragma region Invoke

		TReturn
		operator ()(TArgs&&... a_args) const
		{
			return Invoke(std::forward<TArgs>(a_args)...);
		}

		template<typename = void>
			requires (std::is_void_v<TReturn>)
		void
		Invoke(TArgs&&... a_args) const
		{
			if (m_obj == nullptr)
			{
				(void) m_fn(std::forward<TArgs>(a_args)...);
				return;
			}

			using MemberThunkFn = TReturn(void*, TArgs...);
			MemberThunkFn* fn;
			std::memcpy(&fn, &m_fn, sizeof(fn));
			(void) fn(m_obj, std::forward<TArgs>(a_args)...);
		}

		template<typename = void>
			requires (!std::is_void_v<TReturn>)
		void
		Invoke(TArgs&&... a_args) const
		{
			if (m_obj == nullptr)
			{
				return m_fn(std::forward<TArgs>(a_args)...);
			}

			using MemberThunkFn = TReturn(void*, TArgs...);
			MemberThunkFn* fn;
			std::memcpy(&fn, &m_fn, sizeof(fn));
			return fn(m_obj, std::forward<TArgs>(a_args)...);
		}
	#pragma endregion Invoke

	private:
		void* m_obj = nullptr;
		Fn* m_fn = nullptr;
	};

	template<auto Function>
	Delegate<Traits::FunctionSignatureFromPointer_T<decltype(Function)>>
	CreateDelegate()
	{
		using TDelegate = Delegate<Traits::FunctionSignatureFromPointer_T<decltype(Function)>>;
		return TDelegate::Create(Function);
	}

	template<auto Function>
	Delegate<Traits::FunctionSignatureFromPointer_T<decltype(Function)>>
	CreateDelegate(Traits::MemberFunctionBaseClass_T<decltype(Function)>* a_obj)
	{
		using TDelegate = Delegate<Traits::FunctionSignatureFromPointer_T<decltype(Function)>>;
		return TDelegate::Create(a_obj, Function);
	}

	#define _DELEGATE_MEMBER_CREATE_DELEGATE_FUNC(_cv_ref_noexcept_) \
		template<typename TObj, typename TClass, typename TReturn, typename... TArgs> \
			requires std::is_base_of_v<TClass, TObj> \
		Delegate<TReturn(TArgs...)> \
		CreateDelegate(TObj* a_obj, TReturn(TClass::*a_fn)(TArgs...) _cv_ref_noexcept_) \
		{ \
			return Delegate<TReturn(TArgs...)>::Create(a_obj, a_fn); \
		}

	_CLASS_DEFINE_CV_REF_NOEXCEPT(_DELEGATE_MEMBER_CREATE_DELEGATE_FUNC)
	#undef _DELEGATE_MEMBER_CREATE_DELEGATE_FUNC
}

#undef _MEMBER_FUNCTION_POINTER_DECL
