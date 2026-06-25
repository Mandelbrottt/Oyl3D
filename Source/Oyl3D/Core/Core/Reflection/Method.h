#pragma once

#include "Function.h"
#include "ReflectionParams.h"

namespace Oyl::Reflection
{
	namespace Internal
	{
		class ReflectionFactory;
	}

	class Method : public Function
	{
		friend Internal::ReflectionFactory;

	protected:
		explicit
		Method(const Internal::MethodParams& a_params);

	public:
		bool
		IsVirtual() const
		{
			return m_isVirtual;
		}

		bool
		IsPureVirtual() const
		{
			return m_isPureVirtual;
		}

		template<typename TReturn, typename TObject, typename... TArgs>
		TReturn
		InvokeUnsafe(TObject* a_this, TArgs&&... a_args)
		{
			using ThunkFn = TReturn (TObject::*)(TArgs...);

			ThunkFn thunkFn = *reinterpret_cast<ThunkFn*>(&m_rawFnPtr);
			if constexpr (std::is_void_v<TReturn>)
			{
				(a_this->*thunkFn)(std::forward<TArgs>(a_args)...);
			} else
			{
				return (a_this->*thunkFn)(std::forward<TArgs>(a_args)...);
			}

			throw "Unreachable";
		}

	private:
		bool m_isVirtual = false;
		bool m_isPureVirtual = false;
	};
}