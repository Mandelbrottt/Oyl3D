#pragma once

#include <type_traits>

#include "Enums.h"

namespace Spyll
{
#define DESCRIPTOR_ID(_name_) \
	enum class _name_ : uint32_t \
	{ \
		Invalid = static_cast<std::underlying_type_t<_name_>>(-1)\
	}; \
	\
	inline \
	bool \
	operator !(_name_ a_id) \
	{ \
		return a_id == _name_::Invalid; \
	}

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

	inline
	bool
	operator !(Reflection::AccessSpecifier a_spec)
	{
		return a_spec == Reflection::AccessSpecifier::None;
	}

	inline
	bool
	operator !(Reflection::ConstructorType a_type)
	{
		return a_type == Reflection::ConstructorType::None;
	}
}

#undef JSON_ENUM_HELPER_FNS