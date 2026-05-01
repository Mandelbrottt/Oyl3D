#pragma once

#include "Core/Reflection/Type.h"

namespace Oyl::Reflection::Internal
{
	struct TypeParams;
	struct FieldParams;
	struct FunctionParams;

	class ReflectionFactory
	{
		ReflectionFactory() = delete;

	public:
		static
		::Oyl::Reflection::Type*
		CreateType(const TypeParams& a_params);

		static
		::Oyl::Reflection::Field*
		AddFieldToType(::Oyl::Reflection::Type* a_type, const FieldParams& a_params);

		static
		::Oyl::Reflection::MemberFunction*
		AddFunctionToType(::Oyl::Reflection::Type* a_type, const MemberFunctionParams& a_params);
	};

	struct TypeParams
	{
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
