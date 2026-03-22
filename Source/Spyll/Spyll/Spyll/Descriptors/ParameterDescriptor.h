#pragma once

#include <string>

#include "DescriptorEnums.h"

namespace Spyll
{
	struct ParameterDescriptor
	{
		std::string name;

		DescriptorId id = DescriptorId::Invalid;
		DescriptorId type = DescriptorId::Invalid;
		DescriptorId owningType = DescriptorId::Invalid;

		uint32_t offset = uint32_t(-1);
		
		bool isConst     : 1;
		bool isReference : 1;
		bool isPointer   : 1;
	};
}