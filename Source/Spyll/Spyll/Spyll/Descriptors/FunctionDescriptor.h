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

		AccessSpecifier accessSpecifier : 2;
		ConstructorType constructorType : 2;

		bool isVirtual     : 1;
		bool isPureVirtual : 1;
		bool isOverride    : 1;

		bool isStatic   : 1;
		bool isConst    : 1;
		bool isVolatile : 1;

		bool isReturnConst     : 1;
		bool isReturnVolatile  : 1;
		bool isReturnReference : 1;
		bool isReturnPointer   : 1;
	};
}
