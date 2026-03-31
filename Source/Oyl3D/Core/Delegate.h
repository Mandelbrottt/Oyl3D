#pragma once

#include <type_traits>
#include <cstdio>

template<typename TInvalid>
class Delegate;

template<typename TReturn, typename... TArgs>
class Delegate<TReturn(TArgs...)>
{
	using Fn = TReturn(TArgs...);

	template<typename T>
	using MemberFn = TReturn (T::*)(TArgs...);

public:
	template<typename T>
	void
	Set(T* a_obj, MemberFn<T> a_fn)
	{
		m_obj = reinterpret_cast<void*>(a_obj);
		void* voidPtrToFn = *reinterpret_cast<void**>(&a_fn);
		m_fn = reinterpret_cast<Fn*>(voidPtrToFn);
	}

	void
	Set(Fn a_fn)
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
			} else
			{
				auto fn = reinterpret_cast<ThunkFn*>(m_fn);
				(void) fn(m_obj, std::forward<TArgs>(a_args)...);
			}
		} else {
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

class Something
{
public:
	void
	__stdcall
	Brother()
	{
		printf("hehe %d\n", a);
	}

	int a;
};

using BrotherFn = void(*)(Something*);

void
Brother()
{
	printf("Something!\n");
}

void Foo()
{
	Something s;
	s.a = 8;

	Delegate<void()> d;
	d.Set(&s, &Something::Brother);
	d.Invoke();
	d();

	d.Set(Brother);
	d.Invoke();
	d();
}
