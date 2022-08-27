#pragma once

namespace Rearm::Reflection
{
	namespace Detail
	{
		template<typename TReturn, typename T, typename... TArgs>
		struct function_call_helper
		{
			static
			TReturn
			call(UnknownFunctionPointer a_function, T a_obj, TArgs ...a_args)
			{
				using RawT = std::remove_pointer_t<T>;
				
				TReturn (RawT::* memberFunctionPointer)(TArgs ...);
			
				assert(sizeof(memberFunctionPointer) <= sizeof(a_function));
				std::memcpy(&memberFunctionPointer, &a_function, sizeof(memberFunctionPointer));
				
				return (a_obj->*memberFunctionPointer)(std::forward<TArgs>(a_args)...);
			}
		};

		template<typename TReturn, typename... TArgs>
		struct function_call_helper<TReturn, std::nullptr_t, TArgs...>
		{
			static
			TReturn
			call(UnknownFunctionPointer a_function, std::nullptr_t, TArgs ...a_args)
			{
				TReturn (*functionPointer)(TArgs ...);
			
				assert(sizeof(functionPointer) <= sizeof(a_function));
				std::memcpy(&functionPointer, &a_function, sizeof(functionPointer));
				
				return (functionPointer)(std::forward<TArgs>(a_args)...);
			}
		};
	}

	template<typename TReturn, typename T, typename... TArgs>
	TReturn
	Function::Call(T a_obj, TArgs ...a_args) const
	{
		static_assert(std::is_pointer_v<T> || std::is_same_v<T, std::nullptr_t>);

		// TODO: Add runtime type checks in debug

		return Detail::function_call_helper<TReturn, T, TArgs...>::call(
			m_function,
			a_obj,
			std::forward<TArgs>(a_args)...
		);
	}
}
