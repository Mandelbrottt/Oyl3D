#pragma once

#include <string>

#include "DescriptorEnums.h"

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
