#pragma once

namespace Oyl::Reflection
{
	class Type;

	namespace Internal
	{
		class ReflectionFactory;
	}

	struct Attribute
	{
		friend class Internal::ReflectionFactory;

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
