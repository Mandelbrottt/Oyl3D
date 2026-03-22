#pragma once

#include <string>

#include "DescriptorEnums.h"

namespace Spyll
{
	struct FieldDescriptor
	{
		std::string name;

		DescriptorId id = DescriptorId::Invalid;
		DescriptorId type = DescriptorId::Invalid;
		DescriptorId owningType = DescriptorId::Invalid;
		uint32_t offsetInBits = uint32_t(-1);

		AccessSpecifier accessSpecifier : 2 = AccessSpecifier::None;

		bool isConst     : 1;
		bool isVolatile  : 1;
		bool isReference : 1;
		bool isPointer   : 1;
	};
}
