#pragma once

#include <string>

#include "Descriptors/DescriptorId.h"

namespace Spyll
{
	struct FunctionDescriptor
	{
		DescriptorId id = DescriptorId::Invalid;
		std::string name;

		DescriptorId owningType = DescriptorId::Invalid;
		
		DescriptorId returnType = DescriptorId::Invalid;
	};
}
