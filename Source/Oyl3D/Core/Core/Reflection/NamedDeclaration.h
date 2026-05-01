#pragma once

#include <string>

namespace Oyl::Reflection
{
	namespace Internal
	{
		class AssemblyFactory;
	}

	class NamedDeclaration
	{
		friend Internal::AssemblyFactory;

	protected:
		NamedDeclaration() = default;

	public:
		virtual
		~NamedDeclaration() = default;

		NamedDeclaration(const NamedDeclaration&) = delete;
		NamedDeclaration&
		operator =(const NamedDeclaration&) = delete;

		NamedDeclaration(NamedDeclaration&&) = delete;
		NamedDeclaration&
		operator =(NamedDeclaration&&) = delete;

		std::string_view
		GetName() const
		{
			return m_name;
		}

		std::string_view
		GetQualifiedName() const
		{
			return m_qualifiedName;
		}

	private:
		std::string m_name;
		std::string m_qualifiedName;
	};
}