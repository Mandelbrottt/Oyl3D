#pragma once

#include <string_view>

#if defined(__REFLECT_GENERATE__) || defined (__INTELLISENSE__)
#	define Attr(...) [[clang::annotate("__ATTR__", __VA_ARGS__)]]
#else
#	define Attr(...)
#endif

namespace Oyl::Reflection
{
	class Type;

	namespace Internal
	{
		class ReflectionFactory;
	}

	struct Attribute
	{
		friend Internal::ReflectionFactory;

		virtual
		~Attribute() = default;

		const Type*
		GetType() const
		{
			return m_type;
		}

	private:
		Type* m_type = nullptr;
	};

	struct Enable : Attribute {};

	struct Disable : Attribute {};

	struct DisplayName : Attribute
	{
		constexpr
		explicit
		DisplayName(std::string_view a_name)
			: name(a_name) {}

		std::string_view name;
	};
}

#ifdef OYL_ATTR_TYPEDEF
namespace Oyl
{
	namespace Attr = Reflection::Attr;
}
#endif
