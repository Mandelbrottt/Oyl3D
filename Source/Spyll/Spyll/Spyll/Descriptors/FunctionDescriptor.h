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
	};
}
