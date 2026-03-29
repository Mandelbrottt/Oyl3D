#pragma once

#include <string>

#include "DescriptorEnums.h"

namespace Spyll
{
	struct VariableDescriptor
	{
		std::string name;

		VariableDescriptorId id = VariableDescriptorId::Invalid;
		TypeDescriptorId type = TypeDescriptorId::Invalid;

		// TODO: Find better solution, maybe we DO need to separate vars and params
		TypeDescriptorId ownerType = TypeDescriptorId::Invalid;
		FunctionDescriptorId ownerFunction = FunctionDescriptorId::Invalid;

		AccessSpecifier accessSpecifier;

		bool isConst;
		bool isVolatile;
		bool isReference;
		bool isPointer;
	};
}
