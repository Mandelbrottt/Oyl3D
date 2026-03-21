#pragma once

#include <string>

#include "Descriptors/DescriptorId.h"

namespace Spyll
{
	struct EnumDescriptor
	{
		DescriptorId id;
		
		std::string name;
	};
}
