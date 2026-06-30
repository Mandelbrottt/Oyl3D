#pragma once

namespace Oyl::Reflection
{
	struct Attribute
	{
		virtual
		~Attribute() = default;
	};

	struct EnableAttribute : Attribute {};

	struct DisplayNameAttribute : Attribute
	{
		constexpr
		explicit
		DisplayNameAttribute(std::string_view a_name)
			: name(a_name) {}

		std::string_view name;
	};
}
