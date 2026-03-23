#pragma once

#include <string>

#include "DescriptorEnums.h"
#include "TypeDescriptor.h"

namespace Spyll
{
	struct EnumDescriptor
	{
		std::string name;
		
		EnumDescriptorId id = EnumDescriptorId::Invalid;
		TypeDescriptorId underlyingType = TypeDescriptorId::Invalid;

		std::vector<std::pair<std::string, int64_t>> enumerations;
	};
}
