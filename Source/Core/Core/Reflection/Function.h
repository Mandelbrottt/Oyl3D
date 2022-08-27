#pragma once

#include <cassert>
#include <functional>

#include "TypeId.h"

namespace Rearm::Reflection
{
	namespace Detail
	{
		struct UnknownFunctionPointer
		{
		private:
			class Undefined;

			void (Undefined::*m_mfp)() = nullptr;
		};
	}
	

	/**
	 * \brief Runtime representation of a field member.
	 */
	class Function
	{
	public:
		template<typename TFunctionPointer>
		Function(
			const std::string&         a_name,
			const std::string&         a_displayName,
			const std::string&         a_description,
			TFunctionPointer           a_function
		)
			: m_name(a_name),
			  m_displayName(a_displayName),
			  m_description(a_description)
		{
			assert(sizeof(TFunctionPointer) <= sizeof(Detail::UnknownFunctionPointer));
			std::memcpy(&m_function, &a_function, sizeof(a_function));
		}

		const std::string&
		Name() const
		{
			return m_name;
		}

		const std::string&
		DisplayName() const
		{
			return m_displayName;
		}

		const std::string&
		Description() const
		{
			return m_description;
		}

		template<typename TReturn, typename T, typename... TArgs>
		TReturn
		Call(T a_obj, TArgs ...a_args) const;
		
		template<typename T, typename... TArgs>
		void
		Call(T a_obj, TArgs ...a_args) const
		{
			Call<void>(a_obj, std::forward<TArgs>(a_args)...);
		}
		
	private:
		// The actual name of the field in the source code
		std::string m_name;

		// The name to display to the user in visual applications. Can be user-defined.
		std::string m_displayName;

		// The description of the field. Optionally user-defined, else an empty string.
		std::string m_description;

		Detail::UnknownFunctionPointer m_function;
	};
}

#include "Function.inl"