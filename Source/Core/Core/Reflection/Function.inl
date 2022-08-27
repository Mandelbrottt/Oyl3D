#pragma once
#include "Type.h"
#include "Type.h"
#include "Type.h"

namespace Rearm::Reflection
{
	namespace Detail
	{
		template<typename TFirst, typename... TRest>
		struct emplace_typeid_in_vector_impl;
		
		template<typename TFirst>
		struct emplace_typeid_in_vector_impl<TFirst>
		{
			static
			void
			call(std::vector<TypeId>& a_vector)
			{
				a_vector.emplace_back(GetTypeId<TFirst>());
			}
		};
		
		template<typename TFirst, typename... TRest>
		struct emplace_typeid_in_vector_impl
		{
			static
			void
			call(std::vector<TypeId>& a_vector)
			{
				a_vector.emplace_back(GetTypeId<TFirst>());
				emplace_typeid_in_vector_impl<TRest...>::call(a_vector);
			}
		};
		
		template<typename... TRest>
		struct emplace_typeid_in_vector;
		
		template<>
		struct emplace_typeid_in_vector<>
		{
			static
			void
			call(std::vector<TypeId>& a_vector) { }
		};
		
		template<typename... TRest>
		struct emplace_typeid_in_vector
		{
			static
			void
			call(std::vector<TypeId>& a_vector)
			{
				emplace_typeid_in_vector_impl<TRest...>::call(a_vector);
			}
		};
		
		template<typename TReturn, typename TContaining, typename... TArgs>
		struct function_call_helper
		{
			static
			TReturn
			call(UnknownFunctionPointer a_function, TContaining a_obj, TArgs&& ...a_args)
			{
				using RawT = std::remove_pointer_t<TContaining>;
				
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
			call(UnknownFunctionPointer a_function, std::nullptr_t, TArgs&& ...a_args)
			{
				TReturn (*functionPointer)(TArgs ...);
			
				assert(sizeof(functionPointer) <= sizeof(a_function));
				std::memcpy(&functionPointer, &a_function, sizeof(functionPointer));
				
				return (functionPointer)(std::forward<TArgs>(a_args)...);
			}
		};
	}

	template<typename TReturn, typename TContaining, typename ... TArgs>
	TReturn
	Function::callable_helper<TReturn(TArgs ...), TContaining>::call(
		Detail::UnknownFunctionPointer   a_function,
		TContaining a_obj,
		TArgs&& ...   a_args
	) {
		return Detail::function_call_helper<TReturn, TContaining, TArgs...>::call(
			a_function,
			a_obj,
			std::forward<TArgs>(a_args)...
		);
	}

	//template<typename TReturn, typename TContaining>
	//TReturn
	//Function::callable_helper<TReturn(), TContaining>::call(
	//	Detail::UnknownFunctionPointer a_function,
	//	TContaining                      a_obj
	//)
	//{
	//	return Detail::function_call_helper<TReturn, TContaining>::call(
	//		a_function,
	//		a_obj
	//	);
	//}

	//template<typename TReturn, typename TContaining, typename... TArgs>
	//TReturn
	//Function::Call(TContaining a_obj, TArgs ...a_args) const
	//{
	//#ifndef REARM_RELEASE
	//	const Type& containingType     = Type::Get<TContaining>();
	//	const Type* thisContainingType = Type::TryGet(this->m_containingTypeId);

	//	const Type& returnType     = Type::Get<TReturn>();
	//	const Type* thisReturnType = Type::TryGet(this->m_returnTypeId);

	//	bool containingIsNull = containingType.GetTypeId() == GetTypeId<std::nullptr_t>()
	//	                        && thisContainingType == nullptr;
	//	bool containingIsValid = containingType.GetTypeId() != GetTypeId<std::nullptr_t>()
	//	                         && thisContainingType != nullptr;
	//	bool isContainingConvertible = containingIsValid && thisContainingType->IsConvertibleFrom(&containingType);

	//	bool returnNotNull       = thisReturnType != nullptr;
	//	bool isReturnConvertible = thisReturnType->IsConvertibleFrom(&returnType);
	//
	//	assert(containingIsNull ^ containingIsValid);
	//	if (containingIsValid)
	//		assert(isContainingConvertible);
	//	assert(returnNotNull);
	//	assert(isReturnConvertible);
	//	
	//	std::vector<TypeId> parameterTypeIds;
	//	parameterTypeIds.reserve(num_packed_types_v<TArgs...>);
	//	Detail::emplace_typeid_in_vector<TArgs...>::call(parameterTypeIds);

	//	const std::vector<TypeId>& thisParameterTypeIds = m_parameterTypeIds;
	//	
	//	bool paramLengthsMatch = parameterTypeIds.size() == thisParameterTypeIds.size();
	//	assert(paramLengthsMatch);
	//	
	//	bool areParametersConvertible = [&]
	//	{
	//		auto size = parameterTypeIds.size();
	//		for (int i = 0; i < size; i++)
	//		{
	//			const Type* paramType     = Type::TryGet(parameterTypeIds[i]);
	//			const Type* thisParamType = Type::TryGet(thisParameterTypeIds[i]);
	//			if (paramType == nullptr || thisParamType == nullptr || !thisParamType->IsConvertibleFrom(paramType))
	//			{
	//				return false;
	//			}
	//		}
	//		return true;
	//	}();
	//	assert(areParametersConvertible);
	//#endif
	//	
	//	static_assert(std::is_pointer_v<TContaining> || std::is_same_v<TContaining, std::nullptr_t>);

	//	// TODO: Add runtime type checks in debug

	//	return Detail::function_call_helper<TReturn, TContaining, TArgs...>::call(
	//		m_function,
	//		a_obj,
	//		std::forward<TArgs>(a_args)...
	//	);
	//}

	template<typename TReturn, typename TContaining, typename ... TArgs>
	std::vector<TypeId>
	Function::ParameterTypeIds(TReturn (TContaining::*)(TArgs ...))
	{
		std::vector<TypeId> parameterTypeIds;
		parameterTypeIds.reserve(num_packed_types_v<TArgs...>);
		Detail::emplace_typeid_in_vector<TArgs...>::call(parameterTypeIds);
		return parameterTypeIds;
	}

	template<typename TReturn, typename TContaining, typename ... TArgs>
	std::vector<TypeId>
	Function::ParameterTypeIds(TReturn (TContaining::*)(TArgs ...) const)
	{
		std::vector<TypeId> parameterTypeIds;
		parameterTypeIds.reserve(num_packed_types_v<TArgs...>);
		Detail::emplace_typeid_in_vector<TArgs...>::call(parameterTypeIds);
		return parameterTypeIds;
	}

	template<typename TReturn, typename ... TArgs>
	std::vector<TypeId>
	Function::ParameterTypeIds(TReturn (*)(TArgs ...))
	{
		std::vector<TypeId> parameterTypeIds;
		parameterTypeIds.reserve(num_packed_types_v<TArgs...>);
		Detail::emplace_typeid_in_vector<TArgs...>::call(parameterTypeIds);
		return parameterTypeIds;
	}
}
