#pragma once

#include <algorithm>

#include "Core/Containers/Array.h"
#include "Core/Platform/MemberFunctionPointer.h"
#include "Core/Types/TypeTraits.h"

namespace Oyl
{
	template<Traits::FunctionSignature>
	class Delegate;

	template<typename TReturn, typename... TArgs>
	class Delegate<TReturn(TArgs...)>
	{
	public:
		using FnPtr = TReturn(*)(TArgs...);

		template<typename TObj>
		using ObjFnPtr = TReturn(*)(TObj, TArgs...);

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

		// Templated Function Pointer

		template<FnPtr Function>
		Delegate(FnPtr)
			: Delegate()
		{
			Set<Function>();
		}

		template<FnPtr Function>
		void
		Set(FnPtr)
		{
			m_obj = nullptr;
			m_fn = Function;
		}

		// Templated Function Pointer
		// Function-Like Predicate

		// TReturn(*)(TArgs...)
		template<Traits::CFunctionLike<TArgs...> TPredicate>
			requires requires(TPredicate&& a_functor, TArgs&&... a_args)
			{
				{ a_functor(std::forward<TArgs>(a_args)...) } -> Traits::ConvertibleTo<TReturn>;
			}
		Delegate(TPredicate&& a_functor)
		{
			*this = a_functor;
		}

		// TReturn(*)(TArgs...)
		template<Traits::CFunctionLike<TArgs...> TPredicate>
			requires requires(TPredicate&& a_functor, TArgs&&... a_args)
			{
				{ a_functor(std::forward<TArgs>(a_args)...) } -> Traits::ConvertibleTo<TReturn>;
			}
		Delegate&
		operator =(TPredicate&& a_functor)
		{
			Set(static_cast<FnPtr>(a_functor));
			return *this;
		}

		// TReturn(*)(TArgs...)
		template<Traits::CFunctionLike<TArgs...> TPredicate>
			requires requires(TPredicate&& a_functor, TArgs&&... a_args)
			{
				{ a_functor(std::forward<TArgs>(a_args)...) } -> Traits::ConvertibleTo<TReturn>;
			}
		void
		Set(TPredicate&& a_functor)
		{
			m_obj = nullptr;
			m_fn = static_cast<FnPtr>(a_functor);
		}

		// Function-Like Predicate
		// Object Pointer Function-Like Predicate

		// TReturn(*)(TObjBase&, TArgs...)
		template<typename TObj, typename TPredicate>
			requires Traits::CFunctionLike<TPredicate, typename Traits::FirstArgumentOfSignature<TPredicate>::type, TArgs...>
			         && requires(TObj* a_obj, TPredicate&& a_functor, TArgs&&... a_args)
			         {
				         { a_functor(a_obj, std::forward<TArgs>(a_args)...) } -> Traits::ConvertibleTo<TReturn>;
			         }
		Delegate(TObj* a_obj, TPredicate a_functor)
		{
			using TFirstArg = typename Traits::FirstArgumentOfSignature<TPredicate>::type;
			Set(a_obj, static_cast<ObjFnPtr<TFirstArg>>(a_functor));
		}

		// TReturn(*)(TObjBase&, TArgs...)
		template<typename TObj, typename TPredicate>
			requires Traits::CFunctionLike<TPredicate, typename Traits::FirstArgumentOfSignature<TPredicate>::type, TArgs...>
			         && requires(TObj* a_obj, TPredicate&& a_functor, TArgs&&... a_args)
			         {
				         { a_functor(a_obj, std::forward<TArgs>(a_args)...) } -> Traits::ConvertibleTo<TReturn>;
			         }
		void
		Set(TObj* a_obj, TPredicate a_functor)
		{
			using TFirstArg = typename Traits::FirstArgumentOfSignature<TPredicate>::type;
			using TClass = std::remove_pointer_t<TFirstArg>;
			m_obj = static_cast<TClass*>(a_obj);
			m_fn = reinterpret_cast<FnPtr>(a_functor);
		}

		// TReturn(*)(TObjBase*, TArgs...)
		template<typename TObj, typename TPredicate>
			requires Traits::CFunctionLike<TPredicate, typename Traits::FirstArgumentOfSignature<TPredicate>::type, TArgs...>
			         && requires(TObj& a_obj, TPredicate&& a_functor, TArgs&&... a_args)
			         {
				         { a_functor(a_obj, std::forward<TArgs>(a_args)...) } -> Traits::ConvertibleTo<TReturn>;
			         }
		Delegate(TObj* a_obj, TPredicate a_functor)
		{
			using TFirstArg = typename Traits::FirstArgumentOfSignature<TPredicate>::type;
			Set(a_obj, static_cast<ObjFnPtr<TFirstArg>>(a_functor));
		}

		// TReturn(*)(TObjBase*, TArgs...)
		template<typename TObj, typename TPredicate>
			requires Traits::CFunctionLike<TPredicate, typename Traits::FirstArgumentOfSignature<TPredicate>::type, TArgs...>
			         && requires(TObj& a_obj, TPredicate&& a_functor, TArgs&&... a_args)
			         {
				         { a_functor(a_obj, std::forward<TArgs>(a_args)...) } -> Traits::ConvertibleTo<TReturn>;
			         }
		void
		Set(TObj* a_obj, TPredicate a_functor)
		{
			using TFirstArg = typename Traits::FirstArgumentOfSignature<TPredicate>::type;
			using TClass = std::remove_reference_t<TFirstArg>;
			m_obj = static_cast<TClass*>(a_obj);
			m_fn = reinterpret_cast<FnPtr>(a_functor);
		}

		// Object Pointer Function-Like Predicate
		// Member Function Pointer

		// TReturn(TObj::*)(TArgs...)
		template<typename TObj, typename TMemberFunctionPointer>
			requires Traits::Same<TObj, Traits::TMemberFunctionBaseClass<TMemberFunctionPointer>>
			         && requires(TObj a_obj, TMemberFunctionPointer a_fn, TArgs&&... a_args)
			         {
				         { (a_obj.*a_fn)(std::forward<TArgs>(a_args)...) } -> Traits::ConvertibleTo<TReturn>;
			         }
		Delegate(TObj* a_obj, TMemberFunctionPointer a_fn)
		{
			Set(a_obj, a_fn);
		}

		// TReturn(TObj::*)(TArgs...)
		template<typename TObj, typename TMemberFunctionPointer>
			requires Traits::Same<TObj, Traits::TMemberFunctionBaseClass<TMemberFunctionPointer>>
			         && requires(TObj a_obj, TMemberFunctionPointer a_fn, TArgs&&... a_args)
			         {
				         { (a_obj.*a_fn)(std::forward<TArgs>(a_args)...) } -> Traits::ConvertibleTo<TReturn>;
			         }
		void
		Set(TObj* a_obj, TMemberFunctionPointer a_fn)
		{
			auto mfpData = Platform::MemberFunctionPointerData::Create<TArgs...>(a_obj, a_fn);
			m_obj = mfpData.thisPtr;
			m_fn = reinterpret_cast<FnPtr>(mfpData.thunkPtr);
		}

		// TReturn(TObj::*)(TArgs...)
		template<auto MemberFunctionPtr>
			requires Traits::MemberFunctionPointer<decltype(MemberFunctionPtr)> && requires(Traits::TMemberFunctionBaseClass<decltype(MemberFunctionPtr)> a_obj, TArgs&&... a_args)
			{
				{ (a_obj.*MemberFunctionPtr)(std::forward<TArgs>(a_args)...) } -> Traits::ConvertibleTo<TReturn>;
			}
		void
		Set(Traits::TMemberFunctionBaseClass<decltype(MemberFunctionPtr)>* a_obj)
		{
			Set(a_obj, MemberFunctionPtr);
		}

		// Member Function Pointer
		// Invoke

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

			ObjFnPtr<void*> fn;
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

			ObjFnPtr<void*> fn;
			std::memcpy(&fn, &m_fn, sizeof(fn));
			return fn(m_obj, std::forward<TArgs>(a_args)...);
		}

		// Invoke

	private:
		void* m_obj = nullptr;
		FnPtr m_fn = nullptr;
	};

	template<Traits::FunctionSignature>
	class MulticastDelegate;

	template<typename TReturn, typename... TArgs>
	class MulticastDelegate<TReturn(TArgs...)>
	{
	public:
		using DelegateTy = Delegate<TReturn(TArgs...)>;

		MulticastDelegate() = default;

		NO_COPY(MulticastDelegate);
		DEFAULT_MOVE(MulticastDelegate);

		operator bool() const { return IsValid(); }

		bool
		IsValid() const { return !m_delegates.Empty(); }

		void
		operator +=(DelegateTy a_delegate) const
		{
			// Check if we've already added this delegate
			for (const auto& delegate : m_delegates)
				if (delegate == a_delegate)
					return;

			m_delegates.Add(a_delegate);
		}

		void
		operator -=(DelegateTy a_delegate) const
		{
			for (uint32 i = 0; i < m_delegates.Length(); i++)
				if (m_delegates[i] == a_delegate)
					m_delegates.erase(m_delegates.begin() + i);
		}

		TReturn
		operator ()(TArgs&&... a_args)
		{
			return Invoke(std::forward<TArgs>(a_args)...);
		}

		template<typename = void>
			requires (std::is_void_v<TReturn>)
		void
		Invoke(TArgs&&... a_args)
		{
			for (uint32 i = 0; i < m_delegates.Size(); i++)
				m_delegates[i].Invoke(std::forward<TArgs>(a_args)...);
		}

		template<typename = void>
			requires (!std::is_void_v<TReturn> && std::is_copy_constructible_v<TReturn>)
		Array<TReturn>
		Invoke(TArgs&&... a_args)
		{
			if (m_delegates.empty())
				return {};

			Array<TReturn> result;
			result.Reserve(m_delegates.size());
			for (uint32 i = 0; i < m_delegates.Size(); i++)
				result.Add(m_delegates[i].Invoke(std::forward<TArgs>(a_args)...));
			return result;
		}

	private:
		mutable Array<DelegateTy> m_delegates;
	};
}

#define InstanceDelegate(_member_function_name_) { this, &Traits::TRemovePointer<decltype(this)>::_member_function_name_ }
