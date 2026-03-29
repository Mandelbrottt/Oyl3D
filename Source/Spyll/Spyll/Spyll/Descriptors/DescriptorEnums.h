#pragma once

#include <type_traits>

namespace Spyll
{
#define JSON_ENUM_HELPER_FNS(_name_) \
	template<typename BasicJsonType> \
	void \
	to_json(BasicJsonType& a_json, const _name_& a_id) \
	{ \
		a_json = static_cast<std::underlying_type_t<_name_>>(a_id); \
	} \
	\
	template<typename BasicJsonType> \
	void \
	from_json(const BasicJsonType& a_json, _name_& a_id) \
	{ \
		a_id = static_cast<_name_>(a_json.template get<std::underlying_type_t<_name_>>()); \
	}


#define DESCRIPTOR_ID(_name_) \
	enum class _name_ : uint32_t \
	{ \
		Invalid = static_cast<std::underlying_type_t<_name_>>(-1)\
	}; \
	JSON_ENUM_HELPER_FNS(_name_)

	DESCRIPTOR_ID(TypeDescriptorId)

	DESCRIPTOR_ID(FieldDescriptorId)

	DESCRIPTOR_ID(FunctionDescriptorId)

	DESCRIPTOR_ID(VariableDescriptorId)

	DESCRIPTOR_ID(EnumDescriptorId)

#undef DESCRIPTOR_ID

	namespace Detail
	{
		template<typename DescriptorIdT, typename = std::enable_if_t<std::is_enum_v<DescriptorIdT>, void>>
		DescriptorIdT&
		DescriptorIdRef()
		{
			static DescriptorIdT id = static_cast<DescriptorIdT>(0);
			return id;
		}
	}

	template<typename DescriptorIdT, typename = std::enable_if_t<std::is_enum_v<DescriptorIdT>, void>>
	DescriptorIdT
	GetNewDescriptorId()
	{
		DescriptorIdT& idRef = Detail::DescriptorIdRef<DescriptorIdT>();
		DescriptorIdT newId = idRef;

		// Increment and set the idRef before returning the original value
		auto asUnderlying = static_cast<std::underlying_type_t<DescriptorIdT>>(idRef);
		++asUnderlying;
		idRef = static_cast<DescriptorIdT>(asUnderlying);

		return newId;
	}

	template<typename DescriptorIdT, typename = std::enable_if_t<std::is_enum_v<DescriptorIdT>, void>>
	void
	ResetDescriptorId()
	{
		DescriptorIdT& ref = Detail::DescriptorIdRef<DescriptorIdT>();
		ref = static_cast<DescriptorIdT>(0);
	}

	enum class AccessSpecifier : uint8_t
	{
		None = 0,
		Public,
		Protected,
		Private
	};

	JSON_ENUM_HELPER_FNS(AccessSpecifier)

	enum class ConstructorType : uint8_t
	{
		None,
		Default,
		Copy,
		Move
	};

	JSON_ENUM_HELPER_FNS(ConstructorType)
}

#undef JSON_ENUM_HELPER_FNS