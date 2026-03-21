#pragma once

#include <string>

#include "DescriptorEnums.h"

namespace Spyll
{
	struct FieldDescriptor
	{
		DescriptorId id = DescriptorId::Invalid;
		std::string name;

		DescriptorId type = DescriptorId::Invalid;
		uint32_t offsetInBits = uint32_t(-1);

		bool isStatic    : 1;
		bool isConst     : 1;
		bool isVolatile  : 1;
		bool isReference : 1;
		bool isPointer   : 1;
		
		DescriptorId owningType = DescriptorId::Invalid;
	};
}
