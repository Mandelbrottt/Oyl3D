#pragma once

#include "Invokable.h"
#include "NamedDeclaration.h"
#include "ReflectionParams.h"

namespace Oyl::Reflection
{
	class Type;
	class Assembly;

	namespace Internal
	{
		class ReflectionFactory;
	}

	class Function : public NamedDeclaration, public Invokable
	{
		friend Internal::ReflectionFactory;

	protected:
		explicit
		Function(const Internal::FunctionParams& a_params);

	public:
		virtual
		~Function();

		template<typename TReturn, typename... TArgs>
		TReturn
		InvokeUnsafe(TArgs&&... a_args)
		{
			using ThunkFn = TReturn(*)(TArgs...);

			ThunkFn thunkFn = reinterpret_cast<ThunkFn>(m_functionPtr);
			if constexpr (std::is_void_v<TReturn>)
			{
				thunkFn(std::forward<TArgs>(a_args)...);
			} else
			{
				return thunkFn(std::forward<TArgs>(a_args)...);
			}

			throw "Unreachable";
		}
	};
}
