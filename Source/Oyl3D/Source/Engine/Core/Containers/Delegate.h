#pragma once

#include <algorithm>

#include "Core/Platform/MemberFunctionPointer.h"
#include "Core/Types/TypeTraits.h"
#include "Core/Containers/Array.h"

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
		using MemberFnFromValue = TReturn (Traits::TMemberFunctionBaseClass<decltype(Function)>::*)(TArgs...);

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

		friend
		bool
		operator ==(const Delegate& a_lhs, const Delegate& a_rhs)
		{
			return a_lhs.m_fn == a_rhs.m_fn && a_lhs.m_obj == a_rhs.m_obj;
		}

		friend
		bool
		operator !=(const Delegate& a_lhs, const Delegate& a_rhs)
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
			requires Traits::FunctionPointer<decltype(Function)>
		Delegate(decltype(Function))
			: Delegate()
		{
			Set<Function>();
		}

		template<Traits::CFunctionLike TPredicate>
		Delegate(TPredicate&& a_functor)
		{
			Set(static_cast<Fn*>(a_functor));
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
			requires Traits::FunctionPointer<decltype(Function)>
		void
		Set()
		{
			m_obj = nullptr;
			m_fn = static_cast<Fn*>(Function);
		}

		template<Traits::CFunctionLike TPredicate>
		Delegate&
		operator =(TPredicate&& a_fn)
		{
			Set(static_cast<Fn*>(a_fn));
			return *this;
		}

		Delegate&
		operator =(Fn* a_fn)
		{
			Set(a_fn);
			return *this;
		}

	#pragma endregion Function Pointers
	#pragma region Member Functions

		template<typename TObj, typename TMemberFunctionPointer>
			requires Traits::MemberFunctionCallableWith<TObj, TMemberFunctionPointer>
		Delegate(TObj* a_obj, TMemberFunctionPointer a_fn)
		{
			Set(a_obj, a_fn);
		}

		template<typename TObj, typename TMemberFunctionPointer>
			requires Traits::MemberFunctionCallableWith<TObj, TMemberFunctionPointer>
		static
		Delegate
		Create(TObj* a_obj, TMemberFunctionPointer a_fn)
		{
			Delegate result;
			result.Set(a_obj, a_fn);
			return result;
		}

		template<auto Function>
			requires Traits::MemberFunctionPointer<decltype(Function)>
		static
		Delegate
		Create(Traits::TMemberFunctionBaseClass<decltype(Function)>* a_obj)
		{
			Delegate result;
			result.Set(a_obj, Function);
			return result;
		}

		template<auto Function>
			requires Traits::MemberFunctionPointer<decltype(Function)>
		void
		Set(Traits::TMemberFunctionBaseClass<decltype(Function)>* a_obj)
		{
			Set(a_obj, Function);
		}

		template<typename TObj, typename TMemberFunctionPointer> \
			requires Traits::MemberFunctionCallableWith<TObj, TMemberFunctionPointer>
		void
		Set(TObj* a_obj, TMemberFunctionPointer a_fn)
		{
			using TClass = Traits::TMemberFunctionBaseClass<TMemberFunctionPointer>;
			// static_cast to ensure base class pointer arithmetic is done correctly
			TClass* objAsTClass = static_cast<TClass*>(a_obj);
			Platform::MemberFunctionPointerData mfpdata(objAsTClass, a_fn);
			m_obj = mfpdata.thisPtr;
			m_fn = reinterpret_cast<Fn*>(mfpdata.thunkPtr);
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
	CreateDelegate(Traits::TMemberFunctionBaseClass<decltype(Function)>* a_obj)
	{
		using TDelegate = Delegate<Traits::FunctionSignatureFromPointer_T<decltype(Function)>>;
		return TDelegate::Create(a_obj, Function);
	}

	template<typename TObj, typename TMemberFunctionPointer>
		requires Traits::MemberFunctionCallableWith<TObj, TMemberFunctionPointer>
	Delegate<Traits::FunctionSignatureFromPointer_T<TMemberFunctionPointer>>
	CreateDelegate(TObj* a_obj, TMemberFunctionPointer a_fn)
	{
		return Delegate<Traits::FunctionSignatureFromPointer_T<TMemberFunctionPointer>>::Create(a_obj, a_fn);
	}

	template<Traits::FunctionSignature>
	class MulticastDelegate;

	template<typename TReturn, typename... TArgs>
	class MulticastDelegate<TReturn(TArgs...)>
	{
	public:
		using DelegateType = Delegate<TReturn(TArgs...)>;

		MulticastDelegate() = default;

		MulticastDelegate&
		operator +=(DelegateType a_delegate)
		{
			// Check if we've already added this delegate
			for (const auto& delegate : m_delegates)
				if (delegate == a_delegate)
					return *this;

			m_delegates.Add(a_delegate);
			return *this;
		}

		MulticastDelegate&
		operator -=(DelegateType a_delegate)
		{
			for (uint32 i = 0; i < m_delegates.Length(); i++)
				if (m_delegates[i] == a_delegate)
					m_delegates.erase(m_delegates.begin() + i);
			return *this;
		}

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
			for (const auto& delegate : m_delegates)
				delegate.Invoke(std::forward<TArgs>(a_args)...);
		}

		template<typename = void>
			requires (!std::is_void_v<TReturn> && std::is_copy_constructible_v<TReturn>)
		Array<TReturn>
		Invoke(TArgs&&... a_args) const
		{
			if (m_delegates.empty())
				return {};

			Array<TReturn> result;
			result.Reserve(m_delegates.size());
			for (const auto& delegate : m_delegates)
				result.emplace_back(delegate.Invoke(std::forward<TArgs>(a_args)...));
			return result;
		}

	private:
		Array<DelegateType> m_delegates;
	};
}
