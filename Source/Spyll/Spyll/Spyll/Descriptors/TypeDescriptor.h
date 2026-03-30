#pragma once

#include <string>

#include "DescriptorEnums.h"

namespace Spyll
{
	struct BaseTypeDescriptor
	{
		TypeDescriptorId type = TypeDescriptorId::Invalid;

		Reflection::AccessSpecifier accessSpecifier;
		bool isVirtual;
	};

	struct TypeDescriptor
	{
		std::string name;

		TypeDescriptorId id = TypeDescriptorId::Invalid;

		uint32_t sizeInBits = 0;
		uint8_t alignment = 0;

		bool isComposite;
		bool isStruct;

		bool isOpaque;

		std::vector<BaseTypeDescriptor> baseTypes;
	};
}
