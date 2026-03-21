#pragma once

namespace Spyll
{
	struct FieldDescriptor
	{
		DescriptorId id;

		std::string name;
		uint32_t owningType;
	};
}
