#pragma once

#include <string>

#include "DescriptorEnums.h"

namespace Spyll
{
	struct EnumDescriptor
	{
		std::string name;
		
		EnumDescriptorId id = EnumDescriptorId::Invalid;

		std::vector<std::pair<std::string, int32_t>> enumerations;
	};
}
