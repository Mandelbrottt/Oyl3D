#pragma once

#include <string>

#include "DescriptorEnums.h"

namespace Spyll
{
	struct VariableDescriptor
	{
		using AccessSpecifier = Reflection::AccessSpecifier;

		std::string name;

		VariableDescriptorId id = VariableDescriptorId::Invalid;
		TypeDescriptorId type = TypeDescriptorId::Invalid;

		TypeDescriptorId ownerType = TypeDescriptorId::Invalid;
		uint32_t offsetInBits = uint32_t(-1);

		FunctionDescriptorId ownerFunction = FunctionDescriptorId::Invalid;

		AccessSpecifier accessSpecifier = AccessSpecifier::None;

		bool isConst;
		bool isVolatile;
		bool isReference;
		bool isPointer;
	};
}
