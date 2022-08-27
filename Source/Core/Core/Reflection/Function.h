#pragma once

#include <cassert>

#include "TypeId.h"
#include "TypeTraits.h"

namespace Rearm::Reflection
{
	class Type;
	
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
		);

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

		TypeId
		ContainingTypeId() const
		{
			return m_containingTypeId;
		}

		const Type&
		ContainingType() const;

		TypeId
		ReturnTypeId() const
		{
			return m_returnTypeId;
		}
		
		const Type&
		ReturnType() const;

		const std::vector<TypeId>&
		ParameterTypeIds() const
		{
			return m_parameterTypeIds;
		}

		std::vector<const Type*>
		ParameterTypes() const;

		template<typename TCallable, typename TContaining, typename... TArgs>
		decltype(auto)
		Call(TContaining a_obj, TArgs&& ...a_args) const;

	private:
		// The actual name of the field in the source code
		std::string m_name;

		// The name to display to the user in visual applications. Can be user-defined.
		std::string m_displayName;

		// The description of the field. Optionally user-defined, else an empty string.
		std::string m_description;

		TypeId m_containingTypeId;

		TypeId m_returnTypeId;

		// TODO: Make custom parameter type to replace TypeId
		std::vector<TypeId> m_parameterTypeIds;

		Detail::UnknownFunctionPointer m_functionPointer;
	};
}

#include "Function.inl"