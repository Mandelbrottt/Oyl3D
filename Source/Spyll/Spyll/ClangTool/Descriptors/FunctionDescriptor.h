#pragma once

#include <Spyll/Spyll/TypeId.h>

namespace Spyll
{
	struct FunctionDescriptor
	{
		DescriptorId id;
		
		std::string name;
		Reflection::TypeId owningType;
	};
}
