#pragma once

#include "TypeId.h"

namespace Oyl::Reflection
{
	class Assembly;
	class Type;

	enum class AccessSpecifier : uint8
	{
		Global = 0,
		Public,
		Protected,
		Private
	};
}

namespace Oyl::Reflection::Internal
{
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

		uint32 size;
		uint8 alignment;

		bool isComposite;
		bool isStruct;

		bool isOpaque;
	};

	struct NamedDeclarationParams
	{
		std::string_view qualifiedName;
		std::string_view name;
	};

	struct VariableParams : NamedDeclarationParams
	{
		Type* type;
		Type* parentType;
		AccessSpecifier accessSpecifier;
	};

	struct FieldParams : VariableParams
	{
		uint32 offsetInBits;
		bool isConst;
	};

	struct InvokableParams
	{
		Type* returnType;

		void* functionPtr;
	};

	struct FunctionParams : NamedDeclarationParams, InvokableParams
	{
		Type* parentType;
		AccessSpecifier accessSpecifier;
	};

	struct MethodParams : FunctionParams
	{
		bool isConst;
		bool isVirtual;
		bool isPureVirtual;
	};

	struct ArgumentParams
	{
		Type* type;
		std::string name;
	};

	struct EnumParams : NamedDeclarationParams
	{
		Type* underlyingType;

		size_t nValues;
	};

	struct EnumValueParams
	{
		std::string_view identifier;
		int32 value;
	};
}