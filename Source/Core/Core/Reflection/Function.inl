#pragma once
#include "Type.h"
#include "Type.h"
#include "Type.h"

namespace Rearm::Reflection
{
	namespace Detail
	{
		template<int N, typename TFirst, typename... TRest>
		struct emplace_typeid_in_array_impl;
		
		template<int N, typename TFirst>
		struct emplace_typeid_in_array_impl<N, TFirst>
		{
			static
			void
			call(std::array<TypeId, N>& a_array, int a_position)
			{
				a_array[a_position] = GetTypeId<TFirst>();
			}
		};
		
		template<int N, typename TFirst, typename... TRest>
		struct emplace_typeid_in_array_impl
		{
			static
			void
			call(std::array<TypeId, N>& a_array, int a_position)
			{
				a_array[a_position] = GetTypeId<TFirst>();
				emplace_typeid_in_array_impl<N, TRest...>::call(a_array, a_position + 1);
			}
		};
		
		template<int N, typename... TRest>
		struct emplace_typeid_in_array;
		
		template<int N>
		struct emplace_typeid_in_array<N>
		{
			static
			void
			call(std::array<TypeId, N>& a_array) { }
		};
		
		template<int N, typename... TRest>
		struct emplace_typeid_in_array
		{
			static
			void
			call(std::array<TypeId, N>& a_array)
			{
				emplace_typeid_in_array_impl<N, TRest...>::call(a_array, 0);
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
		
		template<typename TCallable, typename TContaining>
		struct callable_helper;

		template<typename TReturn, typename TContaining, typename... TArgs>
		struct callable_helper<TReturn(TArgs ...), TContaining>
		{
			static
			TReturn
			call(
				const Function&        a_function,
				UnknownFunctionPointer a_functionPointer,
				TContaining            a_obj,
				TArgs&& ...            a_args
			)
			{
			#ifndef REARM_RELEASE
				const Type& containingType     = Type::Get<TContaining>();
				const Type* thisContainingType = Type::TryGet(a_function.ContainingTypeId());

				const Type& returnType     = Type::Get<TReturn>();
				const Type* thisReturnType = Type::TryGet(a_function.ReturnTypeId());

				bool containingIsNull = containingType.GetTypeId() == GetTypeId<std::nullptr_t>()
				                        && thisContainingType == nullptr;

				bool containingIsValid = containingType.GetTypeId() != GetTypeId<std::nullptr_t>()
				                         && thisContainingType != nullptr;

				bool isContainingConvertible = containingIsValid
				                               && thisContainingType->IsConvertibleFrom(&containingType);

				bool returnNotNull       = thisReturnType != nullptr;
				bool isReturnConvertible = thisReturnType->IsConvertibleFrom(&returnType);

				assert(containingIsNull ^ containingIsValid);
				if (containingIsValid)
					assert(isContainingConvertible);
				assert(returnNotNull);
				assert(isReturnConvertible);

				constexpr int numParamTypes = num_packed_types_v<TArgs...>;
				std::array<TypeId, numParamTypes> parameterTypeIds;
				emplace_typeid_in_array<numParamTypes, TArgs...>::call(parameterTypeIds);

				const std::vector<TypeId>& thisParameterTypeIds = a_function.ParameterTypeIds();

				bool paramLengthsMatch = parameterTypeIds.size() == thisParameterTypeIds.size();
				assert(paramLengthsMatch);

				bool areParametersConvertible = [&]
				{
					auto size = parameterTypeIds.size();
					for (int i = 0; i < size; i++)
					{
						const Type* paramType     = Type::TryGet(parameterTypeIds[i]);
						const Type* thisParamType = Type::TryGet(thisParameterTypeIds[i]);
						if (paramType == nullptr 
							|| thisParamType == nullptr
							|| !thisParamType->IsConvertibleFrom(paramType))
						{
							return false;
						}
					}
					return true;
				}();
				assert(areParametersConvertible);
			#endif

				return function_call_helper<TReturn, TContaining, TArgs...>::call(
					a_functionPointer,
					a_obj,
					std::forward<TArgs>(a_args)...
				);
			}
		};
		
		template<typename TReturn, typename TContaining, typename... TArgs>
		static
		TypeId
		ContainingTypeIdFromFunctionDefinition(TReturn (TContaining::*)(TArgs ...))
		{
			return GetTypeId<TContaining>();
		}
		
		template<typename TReturn, typename TContaining, typename... TArgs>
		static
		TypeId
		ContainingTypeIdFromFunctionDefinition(TReturn (TContaining::*)(TArgs ...) const)
		{
			return GetTypeId<TContaining>();
		}
		
		template<typename TReturn, typename... TArgs>
		static
		TypeId
		ContainingTypeIdFromFunctionDefinition(TReturn (*)(TArgs ...))
		{
			return TypeId::Null;
		}
		
		template<typename TReturn, typename TContaining, typename... TArgs>
		static
		TypeId
		ReturnTypeIdFromFunctionDefinition(TReturn (TContaining::*)(TArgs ...))
		{
			return GetTypeId<TReturn>();
		}
		
		template<typename TReturn, typename TContaining, typename... TArgs>
		static
		TypeId
		ReturnTypeIdFromFunctionDefinition(TReturn (TContaining::*)(TArgs ...) const)
		{
			return GetTypeId<TReturn>();
		}
		
		template<typename TReturn, typename... TArgs>
		static
		TypeId
		ReturnTypeIdFromFunctionDefinition(TReturn (*)(TArgs ...))
		{
			return GetTypeId<TReturn>();
		}
		
		template<typename TReturn, typename TContaining, typename... TArgs>
		static
		std::vector<TypeId>
		ParameterTypeIdsFromFunctionDefinition(TReturn (TContaining::*)(TArgs ...))
		{
			constexpr int numTypes = num_packed_types_v<TArgs...>;
			std::array<TypeId, numTypes> parameterTypeIds;
			emplace_typeid_in_array<numTypes, TArgs...>::call(parameterTypeIds);

			std::vector<TypeId> result;
			result.reserve(numTypes);
			result.assign(parameterTypeIds.begin(), parameterTypeIds.end());
			return result;
		}

		template<typename TReturn, typename TContaining, typename... TArgs>
		static
		std::vector<TypeId>
		ParameterTypeIdsFromFunctionDefinition(TReturn (TContaining::*)(TArgs ...) const)
		{
			constexpr int numTypes = num_packed_types_v<TArgs...>;
			std::array<TypeId, numTypes> parameterTypeIds;
			emplace_typeid_in_array<numTypes, TArgs...>::call(parameterTypeIds);

			std::vector<TypeId> result;
			result.reserve(numTypes);
			result.assign(parameterTypeIds.begin(), parameterTypeIds.end());
			return result;
		}

		template<typename TReturn, typename... TArgs>
		static
		std::vector<TypeId>
		ParameterTypeIdsFromFunctionDefinition(TReturn (*)(TArgs ...))
		{
			constexpr int numTypes = num_packed_types_v<TArgs...>;
			std::array<TypeId, numTypes> parameterTypeIds;
			emplace_typeid_in_array<numTypes, TArgs...>::call(parameterTypeIds);

			std::vector<TypeId> result;
			result.reserve(numTypes);
			result.assign(parameterTypeIds.begin(), parameterTypeIds.end());
			return result;
		}
	}

	template<typename TFunctionPointer>
	Function::Function(
		const std::string& a_name,
		const std::string& a_displayName,
		const std::string& a_description,
		TFunctionPointer a_function
	): m_name(a_name),
	   m_displayName(a_displayName),
	   m_description(a_description),
	   m_containingTypeId(Detail::ContainingTypeIdFromFunctionDefinition(a_function)),
	   m_returnTypeId(Detail::ReturnTypeIdFromFunctionDefinition(a_function)),
	   m_parameterTypeIds(Detail::ParameterTypeIdsFromFunctionDefinition(a_function))
	{
		assert(sizeof(TFunctionPointer) <= sizeof(Detail::UnknownFunctionPointer));
		std::memcpy(&m_functionPointer, &a_function, sizeof(a_function));
	}

	template<typename TCallable, typename TContaining, typename ... TArgs>
	decltype(auto)
	Function::Call(TContaining a_obj, TArgs&&...a_args) const
	{
		static_assert(std::is_pointer_v<TContaining> || std::is_same_v<TContaining, std::nullptr_t>);

		return Detail::callable_helper<TCallable, TContaining>::call(
			*this,
			m_functionPointer,
			a_obj,
			std::forward<TArgs>(a_args)...
		);
	}
}
