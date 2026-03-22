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
		std::unordered_map<TypeDescriptorId,     const TypeDescriptor*>     types;
		std::unordered_map<FieldDescriptorId,    const FieldDescriptor*>    fields;
		std::unordered_map<FunctionDescriptorId, const FunctionDescriptor*> functions;
		std::unordered_map<VariableDescriptorId, const VariableDescriptor*> variables;
		std::unordered_map<EnumDescriptorId,     const EnumDescriptor*>     enums;
	};
}
