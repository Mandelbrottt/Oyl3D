#pragma once

#include "Enum.h"
#include "ReflectionParams.h"
#include "Type.h"

namespace Oyl::Reflection::Internal
{
	using ReflectionAllocatorFn = void*(*)(std::size_t, std::align_val_t);

	class ReflectionFactory
	{
		ReflectionFactory() = delete;

	public:
		static
		Assembly*
		CreateAssembly(const AssemblyParams& a_params, ReflectionAllocatorFn a_allocate);
		
		static
		Type*
		AddTypeToAssembly(Assembly* a_assembly, const TypeParams& a_params, ReflectionAllocatorFn a_allocate);

		static
		Variable*
		AddVariableToType(Type* a_type, const VariableParams& a_params, ReflectionAllocatorFn a_allocate);

		static
		Field*
		AddFieldToType(Type* a_type, const FieldParams& a_params, ReflectionAllocatorFn a_allocate);

		static
		Function*
		AddFunctionToType(Type* a_type, const FunctionParams& a_params, ReflectionAllocatorFn a_allocate);

		static
		Method*
		AddMethodToType(Type* a_type, const MethodParams& a_params, ReflectionAllocatorFn a_allocate);

		static
		Argument*
		AddArgumentToInvokable(Invokable* a_function, const ArgumentParams& a_params, ReflectionAllocatorFn a_allocate);

		static
		Variable*
		AddGlobalVariableToAssembly(Assembly* a_assembly, const VariableParams& a_params, ReflectionAllocatorFn a_allocate);

		static
		Function*
		AddGlobalFunctionToAssembly(Assembly* a_assembly, const FunctionParams& a_params, ReflectionAllocatorFn a_allocate);

		static
		Enum*
		AddEnumToAssembly(Assembly* a_assembly, const EnumParams& a_params, ReflectionAllocatorFn a_allocate);

		static
		EnumValue*
		AddValueToEnum(Enum* a_enum, const EnumValueParams& a_params, ReflectionAllocatorFn a_allocate);
	};
}
