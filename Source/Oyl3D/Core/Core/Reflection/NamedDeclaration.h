#pragma once

#include <string>

#include "ReflectionParams.h"

namespace Oyl::Reflection
{
	namespace Internal
	{
		class ReflectionFactory;
	}

	class NamedDeclaration
	{
		friend Internal::ReflectionFactory;

	protected:
		explicit
		NamedDeclaration(const Internal::NamedDeclarationParams& a_params);

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
		std::string m_qualifiedName;
		std::string m_name;
	};
}
