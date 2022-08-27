#pragma once

#include <cassert>

#include "TypeId.h"
#include "TypeTraits.h"

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
	
	#define ReflectFunction(_function_, ...) \
		_REFLECT_MEMBERS_ARGUMENT_NAME._ReflectMember(_REFLECT_NAMESPACE Function { \
			/*.debugName           =*/ #_function_, \
			/*.functionName        =*/ _REFLECT_NAMESPACE Pick<_REFLECT_NAMESPACE Name>(0, __VA_ARGS__, _REFLECT_NAMESPACE Name(#_function_)).c_str, \
			/*.functionDescription =*/ _REFLECT_NAMESPACE Pick<_REFLECT_NAMESPACE Description>(0, __VA_ARGS__, _REFLECT_NAMESPACE Description("")).c_str, \
			/*.function            =*/ &_REFLECT_TYPE_ALIAS::_function_, \
		})
	
	/**
	 * \brief Runtime representation of a field member.
	 */
	class Function
	{
	public:
		template<typename TFunctionPointer>
		Function(
			const std::string& a_name,
			const std::string& a_displayName,
			const std::string& a_description,
			TFunctionPointer   a_function
		)
			: m_name(a_name),
			  m_displayName(a_displayName),
			  m_description(a_description),
			  m_containingTypeId(ContainingTypeId(a_function)),
			  m_returnTypeId(ReturnTypeId(a_function)),
			  m_parameterTypeIds(ParameterTypeIds(a_function))
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

		template<typename TReturn, typename TContaining, typename... TArgs>
		TReturn
		Call(TContaining a_obj, TArgs ...a_args) const;
		
		template<typename TContaining, typename... TArgs>
		void
		Call(TContaining a_obj, TArgs ...a_args) const
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

		TypeId m_containingTypeId;

		TypeId m_returnTypeId;

		// Should these be Field types?
		std::vector<TypeId> m_parameterTypeIds;

		Detail::UnknownFunctionPointer m_function;

	private:
		// TODO: Make below functions macros

		template<typename TReturn, typename TContaining, typename... TArgs>
		static
		TypeId
		ContainingTypeId(TReturn (TContaining::*)(TArgs ...))
		{
			return GetTypeId<TContaining>();
		}
		
		template<typename TReturn, typename TContaining, typename... TArgs>
		static
		TypeId
		ContainingTypeId(TReturn (TContaining::*)(TArgs ...) const)
		{
			return GetTypeId<TContaining>();
		}
		
		template<typename TReturn, typename... TArgs>
		static
		TypeId
		ContainingTypeId(TReturn (*)(TArgs ...))
		{
			return TypeId::Null;
		}
		
		template<typename TReturn, typename TContaining, typename... TArgs>
		static
		TypeId
		ReturnTypeId(TReturn (TContaining::*)(TArgs ...))
		{
			return GetTypeId<TReturn>();
		}
		
		template<typename TReturn, typename TContaining, typename... TArgs>
		static
		TypeId
		ReturnTypeId(TReturn (TContaining::*)(TArgs ...) const)
		{
			return GetTypeId<TReturn>();
		}
		
		template<typename TReturn, typename... TArgs>
		static
		TypeId
		ReturnTypeId(TReturn (*)(TArgs ...))
		{
			return GetTypeId<TReturn>();
		}
		
		template<typename TReturn, typename TContaining, typename... TArgs>
		static
		std::vector<TypeId>
		ParameterTypeIds(TReturn (TContaining::*)(TArgs ...));

		template<typename TReturn, typename TContaining, typename... TArgs>
		static
		std::vector<TypeId>
		ParameterTypeIds(TReturn (TContaining::*)(TArgs ...) const);

		template<typename TReturn, typename... TArgs>
		static
		std::vector<TypeId>
		ParameterTypeIds(TReturn (*)(TArgs ...));

		//template<typename TReturn, typename TContaining>
		//static
		//std::vector<TypeId>
		//ParameterTypeIds(TReturn (TContaining::*)())
		//{
		//	return {};
		//}
		//
		//template<typename TReturn, typename TContaining>
		//static
		//std::vector<TypeId>
		//ParameterTypeIds(TReturn (TContaining::*)() const)
		//{
		//	return {};
		//}
		//
		//template<typename TReturn>
		//static
		//std::vector<TypeId>
		//ParameterTypeIds(TReturn (*)())
		//{
		//	return {};
		//}
	};
}

#include "Function.inl"