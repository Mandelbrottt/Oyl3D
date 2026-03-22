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

		AccessSpecifier accessSpecifier : 2;

		bool isConst     : 1;
		bool isVolatile  : 1;
		bool isReference : 1;
		bool isPointer   : 1;
	};
}
