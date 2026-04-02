#pragma once

#include <string_view>

namespace Spyll
{
	struct ReflectionDescriptor;
}

namespace Spyll
{
	void
	WriteReflectionDescriptorToFile(
		const ReflectionDescriptor& a_descriptor,
		std::string_view a_fileName,
		bool a_binary = true
	);

	ReflectionDescriptor
	ReadReflectionDescriptorFromFile(
		std::string_view a_fileName,
		bool a_binary = true
	);
}
