#pragma once

#include "Core/Reflection/Type.h"

namespace Oyl::Reflection::Internal
{
	struct AssemblyParams;
	struct TypeParams;
	struct FieldParams;
	struct FunctionParams;
	struct MemberFunctionParams;

	using ReflectionAllocatorFn = void*(*)(std::size_t, std::align_val_t);

	class ReflectionFactory
	{
		ReflectionFactory() = delete;

	public:
		static
		::Oyl::Reflection::Assembly*
		CreateAssembly(const AssemblyParams& a_params, ReflectionAllocatorFn a_allocate);
		
		static
		::Oyl::Reflection::Type*
		AddTypeToAssembly(::Oyl::Reflection::Assembly* a_assembly, const TypeParams& a_params, ReflectionAllocatorFn a_allocate);

		static
		::Oyl::Reflection::Field*
		AddFieldToType(::Oyl::Reflection::Type* a_type, const FieldParams& a_params, ReflectionAllocatorFn a_allocate);

		static
		::Oyl::Reflection::MemberFunction*
		AddFunctionToType(::Oyl::Reflection::Type* a_type, const MemberFunctionParams& a_params, ReflectionAllocatorFn a_allocate);
	};

	struct AssemblyParams
	{
		std::string_view name;
	};

	struct TypeParams
	{
		Assembly* assembly;

		TypeId typeId;

		std::string_view qualifiedName;
		std::string_view name;

		uint32_t size;
		uint8_t alignment;

		bool isComposite;
		bool isStruct;

		bool isOpaque;
	};

	struct NamedDeclarationParams
	{
		std::string_view qualifiedName;
		std::string_view name;
	};

	struct QualifierInfoParams
	{
		bool isConst;
		bool isVolatile;
	};

	struct VariableParams : NamedDeclarationParams, QualifierInfoParams
	{
		Type* type;
		
		bool isReference;
		bool isPointer;
	};

	struct MemberInfoParams
	{
		Type* ownerType;
		
		AccessSpecifier accessSpecifier;
	};

	struct FieldParams : VariableParams, MemberInfoParams
	{
		uint32_t offsetInBits;
	};

	struct FunctionParams : NamedDeclarationParams
	{
		Type* returnType;

		bool isStatic;
		bool isConst;
		bool isVolatile;

		bool isReturnConst;
		bool isReturnVolatile;
		bool isReturnReference;
		bool isReturnPointer;
	};

	struct MemberFunctionParams : FunctionParams, MemberInfoParams
	{
		ConstructorType constructorType;

		bool isVirtual;
		bool isPureVirtual;
		bool isOverride;
	};
}
