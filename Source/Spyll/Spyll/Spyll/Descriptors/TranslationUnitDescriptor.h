#pragma once

#include <unordered_map>

#include "DescriptorEnums.h"
#include "EnumDescriptor.h"
#include "FieldDescriptor.h"
#include "FunctionDescriptor.h"
#include "TypeDescriptor.h"
#include "VariableDescriptor.h"

namespace Spyll
{
	struct TranslationUnitDescriptor
	{
		std::vector<TypeDescriptor>     types;
		std::vector<FieldDescriptor>    fields;
		std::vector<FunctionDescriptor> functions;
		std::vector<VariableDescriptor> variables;
		std::vector<EnumDescriptor>     enums;
	};

	extern
	TranslationUnitDescriptor
	CreateMergedTranslationUnitDescriptor(
		const std::vector<TranslationUnitDescriptor>& a_tuDescriptors
	);
}
