#pragma once

#include <string>

#include "DescriptorEnums.h"

namespace Spyll
{
	struct TypeDescriptor
	{
		std::string name;

		TypeDescriptorId id = TypeDescriptorId::Invalid;

		uint32_t sizeInBits = 0;
		uint8_t alignment = 0;

		bool isComposite : 1 = false;
		bool isStruct    : 1 = false;

		bool isOpaque : 1 = true;

		std::vector<FieldDescriptorId> fields;
		std::vector<VariableDescriptorId> variables; // Static members
		std::vector<FunctionDescriptorId> functions;
	};
}
