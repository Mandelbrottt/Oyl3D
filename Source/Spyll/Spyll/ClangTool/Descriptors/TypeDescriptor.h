#pragma once

#include <string>

#include "Descriptors/DescriptorId.h"

namespace Spyll
{
	struct TypeDescriptor
	{
		DescriptorId id = DescriptorId::Invalid;

		std::string name;
		uint32_t size = 0;
		bool isComposite = false;
		bool isStruct = false;

		std::vector<DescriptorId> fields;
		std::vector<DescriptorId> functions;
	};
}
