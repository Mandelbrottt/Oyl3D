#pragma once

#include <type_traits>

namespace Spyll
{
	enum class DescriptorId : uint32_t
	{
		Invalid = static_cast<std::underlying_type_t<DescriptorId>>(-1)
	};

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