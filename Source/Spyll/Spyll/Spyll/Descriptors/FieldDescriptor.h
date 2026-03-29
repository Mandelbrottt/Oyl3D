#pragma once

#include <string>

#include "DescriptorEnums.h"

namespace Spyll
{
	struct FieldDescriptor
	{
		std::string name;

		FieldDescriptorId id = FieldDescriptorId::Invalid;
		TypeDescriptorId type = TypeDescriptorId::Invalid;

		TypeDescriptorId ownerType = TypeDescriptorId::Invalid;
		uint32_t offsetInBits = uint32_t(-1);

		AccessSpecifier accessSpecifier = AccessSpecifier::None;

		bool isConst;
		bool isVolatile;
		bool isReference;
		bool isPointer;
	};
}
