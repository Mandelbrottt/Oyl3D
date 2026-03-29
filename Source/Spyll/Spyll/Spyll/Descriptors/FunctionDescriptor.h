#pragma once

#include <string>

#include "DescriptorEnums.h"

namespace Spyll
{
	struct FunctionDescriptor
	{
		std::string name;

		FunctionDescriptorId id = FunctionDescriptorId::Invalid;
		TypeDescriptorId returnType = TypeDescriptorId::Invalid;

		TypeDescriptorId ownerType = TypeDescriptorId::Invalid;

		AccessSpecifier accessSpecifier;
		ConstructorType constructorType;

		bool isVirtual;
		bool isPureVirtual;
		bool isOverride;

		bool isStatic;
		bool isConst;
		bool isVolatile;

		bool isReturnConst;
		bool isReturnVolatile;
		bool isReturnReference;
		bool isReturnPointer;
	};
}
