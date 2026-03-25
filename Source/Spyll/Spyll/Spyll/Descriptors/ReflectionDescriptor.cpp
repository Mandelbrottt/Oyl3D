#include "ReflectionDescriptor.h"

#include <unordered_map>

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

	struct MergedReflectionDescriptor
	{
		std::unordered_map<std::string_view, TypeDescriptor> mergedTypes;
		std::unordered_map<std::string_view, FieldDescriptor> mergedFields;
		std::unordered_map<std::string_view, FunctionDescriptor> mergedFunctions;
		std::unordered_map<std::string_view, VariableDescriptor> mergedVariables;
		std::unordered_map<std::string_view, EnumDescriptor> mergedEnums;

		void
		ReconnectMergedTypes(const ReflectionDescriptor& originalDescriptor);

		void
		ReconnectMergedFields(const ReflectionDescriptor& originalDescriptor);

		void
		ReconnectMergedFunctions(const ReflectionDescriptor& originalDescriptor);

		void
		ReconnectMergedVariables(const ReflectionDescriptor& originalDescriptor);

		void
		ReconnectMergedEnums(const ReflectionDescriptor& originalDescriptor);;
	};
}

namespace Spyll
{
	ReflectionDescriptor
	CreateMergedReflectionDescriptor(
		const std::vector<ReflectionDescriptor>& a_reflectionDescriptors
	)
	{
		if (a_reflectionDescriptors.size() == 0)
		{
			return {};
		}

		if (a_reflectionDescriptors.size() == 1)
		{
			return a_reflectionDescriptors.front();
		}

		MergedReflectionDescriptor merged;

		// Iterate over the TranslationUnitDescriptors and add unique instances of each declaration to their
		// Respective maps. Maps are keyed by fully qualified names
		for (const auto& reflectionDescriptor : a_reflectionDescriptors)
		{
			MergeDescriptorListIntoMergedMap(reflectionDescriptor.types, &merged.mergedTypes);
			MergeDescriptorListIntoMergedMap(reflectionDescriptor.fields, &merged.mergedFields);
			MergeDescriptorListIntoMergedMap(reflectionDescriptor.functions, &merged.mergedFunctions);
			MergeDescriptorListIntoMergedMap(reflectionDescriptor.variables, &merged.mergedVariables);
			MergeDescriptorListIntoMergedMap(reflectionDescriptor.enums, &merged.mergedEnums);
		}

		// Reconnect the merged types, using the original TranslationUnitDescriptor Ids to index into the descriptor
		// arrays
		// We need to do this step in a separate iteration pass so that we know all the types are set up
		for (const auto& tuDescriptor : a_reflectionDescriptors)
		{
			merged.ReconnectMergedTypes(tuDescriptor);
			merged.ReconnectMergedFields(tuDescriptor);
			merged.ReconnectMergedFunctions(tuDescriptor);
			merged.ReconnectMergedVariables(tuDescriptor);
			merged.ReconnectMergedEnums(tuDescriptor);
		}

		// Write the merged results to a new TranslationUnitDescriptor 
		ReflectionDescriptor result;
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
		// Iterate the descriptorList, add each descriptor to the merged map, keyed by fully qualified name,
		// And set its ID relative to the then-current size of the merged map
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
		// Move the merged descriptors into the final descriptorList
		// Resize the array and move descriptors into the list indexed by their IDs
		// IDs are guaranteed to be valid indices since they are generated based on the size of the map
		a_descriptorList->resize(a_mergedMap->size());
		for (auto& [_, mergedDescriptor] : *a_mergedMap)
		{
			auto mergedDescriptorId = static_cast<std::underlying_type_t<TypeDescriptorId>>(mergedDescriptor.id);
			a_descriptorList->at(mergedDescriptorId) = std::move(mergedDescriptor);
		}
		a_mergedMap->clear();
	}

	void
	MergedReflectionDescriptor::ReconnectMergedTypes(const ReflectionDescriptor& originalDescriptor)
	{
		(void) originalDescriptor;
	}

	void
	MergedReflectionDescriptor::ReconnectMergedFields(const ReflectionDescriptor& originalDescriptor)
	{
		for (const auto& originalField : originalDescriptor.fields)
		{
			// Get the original type descriptor using the original ID as an index
			auto originalTypeIndex = static_cast<std::underlying_type_t<FieldDescriptorId>>(originalField.type);
			const auto& originalType = originalDescriptor.types.at(originalTypeIndex);

			// Get the original owner type descriptor using the original ID as an index
			auto originalOwningTypeIndex =
				static_cast<std::underlying_type_t<TypeDescriptorId>>(originalField.ownerType);
			const auto& originalOwnerType = originalDescriptor.types.at(originalOwningTypeIndex);

			// Get the merged field and types by name
			auto& mergedField = mergedFields.at(originalField.name);
			const auto& mergedType = mergedTypes.at(originalType.name);
			const auto& mergedOwnerType = mergedTypes.at(originalOwnerType.name);

			// Hook up the new IDs
			mergedField.type = mergedType.id;
			mergedField.ownerType = mergedOwnerType.id;
		}
	}

	void
	MergedReflectionDescriptor::ReconnectMergedFunctions(const ReflectionDescriptor& originalDescriptor)
	{
		for (const auto& originalFunction : originalDescriptor.functions)
		{
			// Get the original return type descriptor using the original ID as an index
			auto originalReturnTypeIndex =
				static_cast<std::underlying_type_t<TypeDescriptorId>>(originalFunction.returnType);
			const auto& originalReturnType = originalDescriptor.types.at(originalReturnTypeIndex);

			// Get the merged function and its return type by name
			auto& mergedFunction = mergedFunctions.at(originalFunction.name);
			auto& mergedReturnType = mergedTypes.at(originalReturnType.name);

			// Hook up the new ID
			mergedFunction.returnType = mergedReturnType.id;

			// If the owner type is valid, hook it up too
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
	MergedReflectionDescriptor::ReconnectMergedVariables(const ReflectionDescriptor& originalDescriptor)
	{
		for (const auto& originalVariable : originalDescriptor.variables)
		{
			// Get the original variable type descriptor using the original ID as an index
			auto typeIndex = static_cast<std::underlying_type_t<TypeDescriptorId>>(originalVariable.type);
			const auto& originalType = originalDescriptor.types.at(typeIndex);

			// Get the merged variable and its type by name
			auto& mergedVariable = mergedVariables.at(originalVariable.name);
			auto& mergedType = mergedTypes.at(originalType.name);

			// Hook up the new ID
			mergedVariable.type = mergedType.id;

			// If the owner type is valid, hook it up too
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
	MergedReflectionDescriptor::ReconnectMergedEnums(const ReflectionDescriptor& originalDescriptor)
	{
		for (const auto& originalEnum : originalDescriptor.enums)
		{
			// Get the original underlying type descriptor using the original ID as an index
			auto originalUnderlyingTypeIndex = static_cast<std::underlying_type_t<TypeDescriptorId>>(originalEnum.
				underlyingType);
			const auto& originalUnderlyingType = originalDescriptor.types.at(originalUnderlyingTypeIndex);

			// Get the merged enum and its underlying type by name
			auto& mergedEnum = mergedEnums.at(originalEnum.name);
			auto& mergedType = mergedTypes.at(originalUnderlyingType.name);

			// Hook up the new ID
			mergedEnum.underlyingType = mergedType.id;
		}
	}
}
