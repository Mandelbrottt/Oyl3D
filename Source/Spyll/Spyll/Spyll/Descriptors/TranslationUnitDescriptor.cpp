#include "TranslationUnitDescriptor.h"

namespace Spyll
{
	TranslationUnitDescriptor
	CreateMergedTranslationUnitDescriptor(
		const std::vector<TranslationUnitDescriptor>& a_tuDescriptors
	)
	{
		std::unordered_map<std::string_view, TypeDescriptor> mergedTypes;
		std::unordered_map<std::string_view, FieldDescriptor> mergedFields;
		std::unordered_map<std::string_view, FunctionDescriptor> mergedFunctions;
		std::unordered_map<std::string_view, VariableDescriptor> mergedVariables;
		std::unordered_map<std::string_view, EnumDescriptor> mergedEnums;

		for (const auto& tuDescriptor : a_tuDescriptors)
		{
			auto mergeDescriptorListIntoMergedMap = [&](const auto& descriptorList, auto& mergedMap)
			{
				for (const auto* descriptor : descriptorList)
				{
					auto [iter, emplaced] = mergedMap.try_emplace(descriptor->name, *descriptor);
					if (emplaced)
					{
						auto& [name, newDescriptor] = *iter;
						using IdT = decltype(newDescriptor.id);
						newDescriptor.id = static_cast<IdT>(std::underlying_type_t<IdT>(mergedTypes.size()) - 1);
					}
				}
			};

			mergeDescriptorListIntoMergedMap(tuDescriptor.types, mergedTypes);
			mergeDescriptorListIntoMergedMap(tuDescriptor.fields, mergedFields);
			mergeDescriptorListIntoMergedMap(tuDescriptor.functions, mergedFunctions);
			mergeDescriptorListIntoMergedMap(tuDescriptor.variables, mergedVariables);
			mergeDescriptorListIntoMergedMap(tuDescriptor.enums, mergedEnums);
		}

		TranslationUnitDescriptor result;

		return {};
	}
}
