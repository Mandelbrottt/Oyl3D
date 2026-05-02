#pragma once

#include <filesystem>
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

		std::string sourceFile;
		uint32_t sourceLine = (uint32_t) -1;
		uint32_t sourceColumn = (uint32_t) -1;

		TypeDescriptorId id = TypeDescriptorId::Invalid;

		uint32_t sizeInBits = 0;
		uint8_t alignment = 0;

		bool isComposite;
		bool isStruct;

		bool isOpaque;

		std::vector<BaseTypeDescriptor> baseTypes;
	};
}
