#pragma once

#include "EnumDescriptor.h"
#include "FunctionDescriptor.h"
#include "TypeDescriptor.h"
#include "VariableDescriptor.h"

namespace Spyll
{
	struct ReflectionDescriptor
	{
		std::vector<TypeDescriptor>     types;
		std::vector<FunctionDescriptor> functions;
		std::vector<VariableDescriptor> variables;
		std::vector<EnumDescriptor>     enums;
	};

	namespace Detail
	{
		extern
		ReflectionDescriptor
		CreateMergedReflectionDescriptor(
			const std::vector<ReflectionDescriptor>& a_reflectionDescriptors
		);
	}
}
