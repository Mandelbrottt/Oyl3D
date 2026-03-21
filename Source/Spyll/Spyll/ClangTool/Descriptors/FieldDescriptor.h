#pragma once

namespace Spyll
{
	struct FieldDescriptor
	{
		DescriptorId id;
		std::string name;

		DescriptorId type;
		uint32_t offset;
		
		DescriptorId owningType;
	};
}
