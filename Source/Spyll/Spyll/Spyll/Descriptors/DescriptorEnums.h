#pragma once

#include <type_traits>

namespace Spyll
{
#define DESCRIPTOR_ID(_name_) \
	enum class _name_ : uint32_t \
	{ \
		Invalid = static_cast<std::underlying_type_t<_name_>>(-1)\
	}

	DESCRIPTOR_ID(TypeDescriptorId);

	DESCRIPTOR_ID(FieldDescriptorId);

	DESCRIPTOR_ID(FunctionDescriptorId);

	DESCRIPTOR_ID(VariableDescriptorId);

	DESCRIPTOR_ID(EnumDescriptorId);

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
		DescriptorIdT  newId = idRef;

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

	inline
	AccessSpecifier
	ToAccessSpecifier(clang::AccessSpecifier Spec)
	{
		if (Spec == clang::AS_public) 
			return AccessSpecifier::Public;
		if (Spec == clang::AS_protected) 
			return AccessSpecifier::Protected;
		if (Spec == clang::AS_private) 
			return AccessSpecifier::Private;

		return AccessSpecifier::None;
	}

	enum class ConstructorType : uint8_t
	{
		None,
		Default,
		Copy,
		Move
	};
}