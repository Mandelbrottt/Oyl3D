#pragma once

namespace Spyll
{
	struct TypeDescriptor
	{
		DescriptorId id;

		std::string name;
		uint32_t size;
		bool isComposite;
		bool isStruct;

		std::vector<DescriptorId> fields;
		std::vector<DescriptorId> functions;
	};
}
