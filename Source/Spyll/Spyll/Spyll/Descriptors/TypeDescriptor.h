#pragma once

#include <string>

#include "DescriptorEnums.h"

namespace Spyll
{
	struct TypeDescriptor
	{
		std::string name;

		DescriptorId id = DescriptorId::Invalid;

		uint32_t sizeInBits = 0;
		uint8_t alignment = 0;

		bool isComposite : 1 = false;
		bool isStruct    : 1 = false;

		std::vector<DescriptorId> fields;
		std::vector<DescriptorId> functions;
	};
}
