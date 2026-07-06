#pragma once

#include "TypeTraits.h"

namespace Oyl
{
	template<typename TSignature>
	class Delegate;

	template<typename TReturn, typename... TArgs>
	class Delegate<TReturn(TArgs...)>
	{
	public:
		using Fn = TReturn(TArgs...);

		template<typename T>
			requires (!Traits::Pointer<T>)
		using MemberFn = TReturn (T::*)(TArgs...);

		Delegate() noexcept {}

		Delegate(const Delegate& a_other) noexcept
		{
			*this = a_other;
		}

		Delegate&
		operator=(const Delegate& a_other) noexcept
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

		template<typename TObj>
			requires (!Traits::Pointer<TObj>)
		static
		Delegate
		Create(TObj* a_obj, MemberFn<TObj> a_fn)
		{
			Delegate result;
			result.Set(a_obj, a_fn);
			return result;
		}

		template<typename TObj>
			requires (!Traits::Pointer<TObj>)
		void
		Set(TObj* a_obj, MemberFn<TObj> a_fn)
		{
			m_obj = reinterpret_cast<void*>(a_obj);
			std::memcpy(&m_fn, &a_fn, sizeof(m_fn));
		}

		void
		Set(Fn* a_fn)
		{
			m_obj = nullptr;
			m_fn = a_fn;
		}

		bool
		IsValid() const
		{
			return m_fn;
		}

		explicit
		operator bool() const
		{
			return IsValid();
		}

		TReturn
		operator ()(TArgs... a_args) const
		{
			return Invoke(a_args...);
		}

		TReturn
		Invoke(TArgs... a_args) const
		{
			using MemberThunkFn = TReturn(void*, TArgs...);

			if constexpr (std::is_void_v<TReturn>)
			{
				if (m_obj == nullptr)
				{
					(void) m_fn(std::forward<TArgs>(a_args)...);
					return;
				}

				MemberThunkFn* fn;
				std::memcpy(&fn, &m_fn, sizeof(fn));
				(void) fn(m_obj, std::forward<TArgs>(a_args)...);
				return;
			} else
			{
				if (m_obj == nullptr)
				{
					return m_fn(std::forward<TArgs>(a_args)...);
				}

				MemberThunkFn* fn;
				std::memcpy(&fn, &m_fn, sizeof(fn));
				return fn(m_obj, std::forward<TArgs>(a_args)...);
			}
		}

	private:
		void* m_obj = nullptr;
		Fn* m_fn = nullptr;
	};
}
