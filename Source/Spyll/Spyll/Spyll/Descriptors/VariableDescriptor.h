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

		AccessSpecifier accessSpecifier : 2 = AccessSpecifier::None;

		bool isConst     : 1;
		bool isVolatile  : 1;
		bool isReference : 1;
		bool isPointer   : 1;
	};
}
