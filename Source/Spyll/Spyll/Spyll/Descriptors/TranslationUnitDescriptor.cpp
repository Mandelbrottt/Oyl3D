#include "TranslationUnitDescriptor.h"

namespace
{
	using namespace Spyll;

	template<typename DescriptorT>
	void
	MergeDescriptorListIntoMergedMap(
		const std::vector<DescriptorT>& a_descriptorList,
		std::unordered_map<std::string_view, DescriptorT>* a_mergedMap
	);

	template<typename DescriptorT>
	void
	MoveDescriptorsFromMergedMapToDescriptorList(
		std::unordered_map<std::string_view, DescriptorT>* a_mergedMap,
		std::vector<DescriptorT>* a_descriptorList
	);

	struct MergedTranslationUnitDescriptor
	{
		std::unordered_map<std::string_view, TypeDescriptor> mergedTypes;
		std::unordered_map<std::string_view, FieldDescriptor> mergedFields;
		std::unordered_map<std::string_view, FunctionDescriptor> mergedFunctions;
		std::unordered_map<std::string_view, VariableDescriptor> mergedVariables;
		std::unordered_map<std::string_view, EnumDescriptor> mergedEnums;

		void
		ReconnectMergedTypes(const TranslationUnitDescriptor& originalDescriptor);

		void
		ReconnectMergedFields(const TranslationUnitDescriptor& originalDescriptor);

		void
		ReconnectMergedFunctions(const TranslationUnitDescriptor& originalDescriptor);

		void
		ReconnectMergedVariables(const TranslationUnitDescriptor& originalDescriptor);

		void
		ReconnectMergedEnums(const TranslationUnitDescriptor& originalDescriptor);;
	};
}

namespace Spyll
{
	TranslationUnitDescriptor
	CreateMergedTranslationUnitDescriptor(
		const std::vector<TranslationUnitDescriptor>& a_tuDescriptors
	)
	{
		MergedTranslationUnitDescriptor merged;

		for (const auto& tuDescriptor : a_tuDescriptors)
		{
			MergeDescriptorListIntoMergedMap(tuDescriptor.types, &merged.mergedTypes);
			MergeDescriptorListIntoMergedMap(tuDescriptor.fields, &merged.mergedFields);
			MergeDescriptorListIntoMergedMap(tuDescriptor.functions, &merged.mergedFunctions);
			MergeDescriptorListIntoMergedMap(tuDescriptor.variables, &merged.mergedVariables);
			MergeDescriptorListIntoMergedMap(tuDescriptor.enums, &merged.mergedEnums);
		}

		for (const auto& tuDescriptor : a_tuDescriptors)
		{
			merged.ReconnectMergedTypes(tuDescriptor);
			merged.ReconnectMergedFields(tuDescriptor);
			merged.ReconnectMergedFunctions(tuDescriptor);
			merged.ReconnectMergedVariables(tuDescriptor);
			merged.ReconnectMergedEnums(tuDescriptor);
		}

		TranslationUnitDescriptor result;
		MoveDescriptorsFromMergedMapToDescriptorList(&merged.mergedTypes, &result.types);
		MoveDescriptorsFromMergedMapToDescriptorList(&merged.mergedFields, &result.fields);
		MoveDescriptorsFromMergedMapToDescriptorList(&merged.mergedFunctions, &result.functions);
		MoveDescriptorsFromMergedMapToDescriptorList(&merged.mergedVariables, &result.variables);
		MoveDescriptorsFromMergedMapToDescriptorList(&merged.mergedEnums, &result.enums);

		return result;
	}
}

namespace
{
	template<typename DescriptorT>
	void
	MergeDescriptorListIntoMergedMap(
		const std::vector<DescriptorT>& a_descriptorList,
		std::unordered_map<std::string_view, DescriptorT>* a_mergedMap
	)
	{
		for (const auto& descriptor : a_descriptorList)
		{
			auto [iter, wasEmplaced] = a_mergedMap->try_emplace(descriptor.name, descriptor);
			if (wasEmplaced)
			{
				auto& [name, newDescriptor] = *iter;
				using IdT = decltype(newDescriptor.id);
				newDescriptor.id = static_cast<IdT>(std::underlying_type_t<IdT>(a_mergedMap->size()) - 1);
			}
		}
	}

	template<typename DescriptorT>
	void
	MoveDescriptorsFromMergedMapToDescriptorList(
		std::unordered_map<std::string_view, DescriptorT>* a_mergedMap,
		std::vector<DescriptorT>* a_descriptorList
	)
	{
		a_descriptorList->resize(a_mergedMap->size());
		for (auto& [_, mergedDescriptor] : *a_mergedMap)
		{
			auto mergedDescriptorId = static_cast<std::underlying_type_t<TypeDescriptorId>>(mergedDescriptor.id);
			a_descriptorList->at(mergedDescriptorId) = std::move(mergedDescriptor);
		}
		a_mergedMap->clear();
	}

	void
	MergedTranslationUnitDescriptor::ReconnectMergedTypes(const TranslationUnitDescriptor& originalDescriptor)
	{
		(void) originalDescriptor;
	}

	void
	MergedTranslationUnitDescriptor::ReconnectMergedFields(const TranslationUnitDescriptor& originalDescriptor)
	{
		for (const auto& originalField : originalDescriptor.fields)
		{
			auto originalTypeIndex = static_cast<std::underlying_type_t<FieldDescriptorId>>(originalField.type);
			const auto& originalType = originalDescriptor.types.at(originalTypeIndex);

			auto originalOwningTypeIndex =
				static_cast<std::underlying_type_t<TypeDescriptorId>>(originalField.ownerType);
			const auto& originalOwnerType = originalDescriptor.types.at(originalOwningTypeIndex);

			auto& mergedField = mergedFields.at(originalField.name);
			auto& mergedType = mergedTypes.at(originalType.name);
			auto& mergedOwnerType = mergedTypes.at(originalOwnerType.name);

			mergedField.type = mergedType.id;
			mergedField.ownerType = mergedOwnerType.id;
		}
	}

	void
	MergedTranslationUnitDescriptor::ReconnectMergedFunctions(const TranslationUnitDescriptor& originalDescriptor)
	{
		for (const auto& originalFunction : originalDescriptor.functions)
		{
			auto originalReturnTypeIndex =
				static_cast<std::underlying_type_t<TypeDescriptorId>>(originalFunction.returnType);
			const auto& originalReturnType = originalDescriptor.types.at(originalReturnTypeIndex);

			auto& mergedFunction = mergedFunctions.at(originalFunction.name);
			auto& mergedReturnType = mergedTypes.at(originalReturnType.name);

			mergedFunction.returnType = mergedReturnType.id;

			if (originalFunction.ownerType != TypeDescriptorId::Invalid)
			{
				auto originalOwningTypeIndex =
					static_cast<std::underlying_type_t<TypeDescriptorId>>(originalFunction.ownerType);
				const auto& originalOwnerType = originalDescriptor.types.at(originalOwningTypeIndex);

				auto& mergedOwnerType = mergedTypes.at(originalOwnerType.name);

				mergedFunction.ownerType = mergedOwnerType.id;
			}
		}
	}

	void
	MergedTranslationUnitDescriptor::ReconnectMergedVariables(const TranslationUnitDescriptor& originalDescriptor)
	{
		for (const auto& originalVariable : originalDescriptor.variables)
		{
			auto typeIndex = static_cast<std::underlying_type_t<TypeDescriptorId>>(originalVariable.type);
			const auto& originalType = originalDescriptor.types.at(typeIndex);

			auto& mergedVariable = mergedVariables.at(originalVariable.name);
			auto& mergedType = mergedTypes.at(originalType.name);

			mergedVariable.type = mergedType.id;

			if (originalVariable.ownerType != TypeDescriptorId::Invalid)
			{
				auto originalOwningTypeIndex
					= static_cast<std::underlying_type_t<TypeDescriptorId>>(originalVariable.ownerType);
				const auto& originalOwnerType = originalDescriptor.types.at(originalOwningTypeIndex);

				auto& mergedOwnerType = mergedTypes.at(originalOwnerType.name);

				mergedVariable.ownerType = mergedOwnerType.id;
			}
		}
	}

	void
	MergedTranslationUnitDescriptor::ReconnectMergedEnums(const TranslationUnitDescriptor& originalDescriptor)
	{
		for (const auto& originalEnum : originalDescriptor.enums)
		{
			auto originalUnderlyingTypeIndex = static_cast<std::underlying_type_t<TypeDescriptorId>>(originalEnum.
				underlyingType);
			const auto& originalUnderlyingType = originalDescriptor.types.at(originalUnderlyingTypeIndex);

			auto& mergedEnum = mergedEnums.at(originalEnum.name);
			auto& mergedType = mergedTypes.at(originalUnderlyingType.name);

			mergedEnum.underlyingType = mergedType.id;
		}
	}
}
