#pragma once

#include <string>

#include "DescriptorEnums.h"

namespace Spyll
{
	struct FunctionDescriptor
	{
		std::string name;

		DescriptorId id = DescriptorId::Invalid;
		DescriptorId owningType = DescriptorId::Invalid;
		DescriptorId returnType = DescriptorId::Invalid;

		uint32_t vtableOffset = uint32_t(-1);

		AccessSpecifier accessSpecifier : 2 = AccessSpecifier::None;
		ConstructorType constructorType : 2 = ConstructorType::None;

		bool isVirtual     : 1;
		bool isPureVirtual : 1;
		bool isOverride    : 1;

		bool isConst    : 1;
		bool isVolatile : 1;

		bool isReturnConst     : 1;
		bool isReturnVolatile  : 1;
		bool isReturnReference : 1;
		bool isReturnPointer   : 1;
	};
}
