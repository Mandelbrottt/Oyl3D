#pragma once

#include "TypeTraits.h"

namespace Oyl
{
	template<typename TSignature>
	class Delegate;

	//template<>
	//class Delegate
	//{
	//public:
	//	Delegate() = delete;

	//	template<typename TSignature>
	//	Delegate<TSignature>
	//	Create(typename Delegate<TSignature>::Fn a_fn)
	//	{
	//		return Delegate<TSignature>::Create(a_fn);
	//	}

	//	template<typename TObj, typename TSignature>
	//	Delegate<TSignature>
	//	Create(TObj* a_obj, typename Delegate<TSignature>::template MemberFn<TObj> a_fn)
	//	{
	//		return Delegate<TSignature>::template Create<TObj>(a_obj, a_fn);
	//	}
	//};

	template<typename TReturn, typename... TArgs>
	class Delegate<TReturn(TArgs...)>
	{
	public:
		using Fn = TReturn(TArgs...);

		template<auto Function>
			requires Traits::MemberFunctionPointer<decltype(Function)>
		using MemberFnFromValue = TReturn (Traits::MemberFunctionClass_T<decltype(Function)>::*)(TArgs...);

		template<typename TClass>
		using MemberFnFromType = TReturn (TClass::*)(TArgs...);

		template<typename TClass>
		using ConstMemberFnFromType = TReturn (TClass::*)(TArgs...) const;

		Delegate() {}

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
		operator=(Delegate&& a_other) noexcept
		{
			std::swap(m_obj, a_other.m_obj);
			std::swap(m_fn, a_other.m_fn);

			return *this;
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

		template<typename TObj, typename TClass>
		Delegate(TObj* a_obj, MemberFnFromType<TClass> a_fn)
		{
			Set(a_obj, a_fn);
		}

		template<typename TObj, typename TClass>
		Delegate(TObj* a_obj, ConstMemberFnFromType<TClass> a_fn)
		{
			Set(a_obj, a_fn);
		}

		template<auto Function>
			requires Traits::MemberFunctionPointer<decltype(Function)>
		static
		Delegate
		Create(Traits::MemberFunctionClass_T<decltype(Function)>* a_obj)
		{
			Delegate result;
			result.Set(a_obj, Function);
			return result;
		}

		template<typename TObj, typename TClass>
			requires std::is_base_of_v<TClass, TObj>
		static
		Delegate
		Create(TObj* a_obj, MemberFnFromType<TClass> a_fn)
		{
			Delegate result;
			result.Set(a_obj, a_fn);
			return result;
		}

		template<typename TObj, typename TClass>
			requires std::is_base_of_v<TClass, TObj>
		static
		Delegate
		Create(TObj* a_obj, ConstMemberFnFromType<TClass> a_fn)
		{
			Delegate result;
			result.Set(a_obj, a_fn);
			return result;
		}

		template<auto Function>
			requires Traits::MemberFunctionPointer<decltype(Function)>
		void
		Set(Traits::MemberFunctionClass_T<decltype(Function)>* a_obj)
		{
			m_obj = reinterpret_cast<void*>(a_obj);
			std::memcpy(&m_fn, Function, sizeof(m_fn));
		}

		template<typename TObj, typename TClass>
			requires std::is_base_of_v<TClass, TObj>
		void
		Set(TObj* a_obj, MemberFnFromType<TClass> a_fn)
		{
			m_obj = reinterpret_cast<void*>(a_obj);
			std::memcpy(&m_fn, &a_fn, sizeof(m_fn));
		}

		template<typename TObj, typename TClass>
			requires std::is_base_of_v<TClass, TObj>
		void
		Set(TObj* a_obj, ConstMemberFnFromType<TClass> a_fn)
		{
			m_obj = reinterpret_cast<void*>(a_obj);
			std::memcpy(&m_fn, &a_fn, sizeof(m_fn));
		}

	#pragma endregion Member Functions
	#pragma region Invoke

		TReturn
		operator ()(TArgs... a_args) const
		{
			return Invoke(a_args...);
		}

		template<typename = void>
			requires (std::is_void_v<TReturn>)
		void
		Invoke(TArgs... a_args) const
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
		Invoke(TArgs... a_args) const
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
	Delegate(decltype(Function)) -> Delegate<Traits::FunctionSignatureFromPointer_T<decltype(Function)>>;

	template<auto Function>
	Delegate(
		Traits::MemberFunctionClass_T<decltype(Function)>* a_obj,
		decltype(Function)
	) -> Delegate<Traits::FunctionSignatureFromPointer_T<decltype(Function)>>;

	template<typename TReturn, typename... TArgs>
		requires (Traits::FunctionPointer<typename Delegate<TReturn(TArgs...)>::Fn>)
	Delegate<TReturn(TArgs...)>
	_CreateDelegate(typename Delegate<TReturn(TArgs...)>::Fn* a_fn)
	{
		return Delegate<TReturn(TArgs...)>::Create(a_fn);
	}

	template<auto Function>
	Delegate<Traits::FunctionSignatureFromPointer_T<decltype(Function)>>
	CreateDelegate()
	{
		using TDelegate = Delegate<Traits::FunctionSignatureFromPointer_T<decltype(Function)>>;
		return TDelegate::Create(Function);
	}

	template<auto Function>
	Delegate<Traits::FunctionSignatureFromPointer_T<decltype(Function)>>
	CreateDelegate(Traits::MemberFunctionClass_T<decltype(Function)>* a_obj)
	{
		using TDelegate = Delegate<Traits::FunctionSignatureFromPointer_T<decltype(Function)>>;
		return TDelegate::Create(a_obj, Function);
	}

	template<typename TObj, typename TClass, typename TReturn, typename... TArgs>
		requires std::is_base_of_v<TClass, TObj>
	Delegate<TReturn(TArgs...)>
	CreateDelegate(TObj* a_obj, typename Delegate<TReturn(TArgs...)>::template MemberFnFromType<TClass> a_fn)
	{
		return Delegate<TReturn(TArgs...)>::Create(a_obj, a_fn);
	}

	template<typename TObj, typename TClass, typename TReturn, typename... TArgs>
		requires std::is_base_of_v<TClass, TObj>
	Delegate<TReturn(TArgs...)>
	CreateDelegate(TObj* a_obj, typename Delegate<TReturn(TArgs...)>::template ConstMemberFnFromType<TClass> a_fn)
	{
		return Delegate<TReturn(TArgs...)>::Create(a_obj, a_fn);
	}
}
