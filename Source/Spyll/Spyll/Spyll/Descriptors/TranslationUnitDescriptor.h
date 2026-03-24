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
		std::vector<const TypeDescriptor*>     types;
		std::vector<const FieldDescriptor*>    fields;
		std::vector<const FunctionDescriptor*> functions;
		std::vector<const VariableDescriptor*> variables;
		std::vector<const EnumDescriptor*>     enums;
	};

	extern
	TranslationUnitDescriptor
	CreateMergedTranslationUnitDescriptor(
		const std::vector<TranslationUnitDescriptor>& a_descriptors
	);
}
