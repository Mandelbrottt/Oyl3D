#pragma once

#include <string>

#include "DescriptorEnums.h"

namespace Spyll
{
	struct VariableDescriptor
	{
		std::string name;

		DescriptorId id = DescriptorId::Invalid;
		DescriptorId type = DescriptorId::Invalid;
		DescriptorId owningType = DescriptorId::Invalid;

		bool isConst     : 1;
		bool isVolatile  : 1;
		bool isReference : 1;
		bool isPointer   : 1;
	};
}
