#pragma once

#include "Declaration.h"

namespace clang
{
	class EnumDecl;
	class EnumConstantDecl;
}

namespace Spyll
{
	class Type;
	class Enum;

	class EnumConstant : public Declaration
	{
	public:
		explicit
		EnumConstant(
			const clang::EnumConstantDecl* a_decl,
			const Enum* a_parent
		);

		virtual
		~EnumConstant();

		std::string_view
		GetIdentifier() const
		{
			return m_identifier;
		}

		int64_t
		GetValue() const
		{
			return m_value;
		}

		const Enum*
		GetParent() const
		{
			return m_parent;
		}

	private:
		std::string m_identifier;
		int64_t m_value;

		const Enum* m_parent;
	};

	class Enum : public Declaration
	{
	public:
		explicit
		Enum(const clang::EnumDecl* a_decl);

		virtual
		~Enum();

	public:
		std::string_view
		GetUnderlyingTypeAsString() const
		{
			return m_underlyingType;
		}

		const std::vector<EnumConstant>&
		GetEntries() const
		{
			return m_entries;
		}

	private:
		std::string m_underlyingType;

		std::vector<EnumConstant> m_entries;
	};
}
