#pragma once

#include "TypeTraits.h"

namespace Oyl
{
	template<typename TInvalid>
	class Delegate;

	template<typename TReturn, typename... TArgs>
	class Delegate<TReturn(TArgs...)>
	{
		using Fn = TReturn(TArgs...);

		template<typename T>
		using MemberFn = TReturn (T::*)(TArgs...);

	public:
		template<typename TObj>
		void
		Set(TObj* a_obj, MemberFn<TObj> a_fn)
		{
			m_obj = reinterpret_cast<void*>(a_obj);
			void* voidPtrToFn = *reinterpret_cast<void**>(&a_fn);
			m_fn = reinterpret_cast<Fn*>(voidPtrToFn);
		}

		void
		Set(Fn* a_fn)
		{
			m_obj = nullptr;
			m_fn = a_fn;
		}

		TReturn
		operator ()(TArgs... a_args)
		{
			return Invoke(a_args...);
		}

		TReturn
		Invoke(TArgs... a_args)
		{
			using ThunkFn = TReturn(void*, TArgs...);

			if constexpr (std::is_void_v<TReturn>)
			{
				if (m_obj == nullptr)
				{
					(void) m_fn(std::forward<TArgs>(a_args)...);
					return;
				}

				auto fn = reinterpret_cast<ThunkFn*>(m_fn);
				(void) fn(m_obj, std::forward<TArgs>(a_args)...);
				return;
			} else
			{
				if (m_obj == nullptr)
				{
					return m_fn(std::forward<TArgs>(a_args)...);
				}

				auto fn = reinterpret_cast<ThunkFn*>(m_fn);
				return fn(m_obj, std::forward<TArgs>(a_args)...);
			}
		}

	private:
		void* m_obj = nullptr;
		Fn* m_fn = nullptr;
	};
}
