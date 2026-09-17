#pragma once

#include "Declaration.h"
#include "Type.h"

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
		friend class ReflectionParser;

	public:
		explicit
		EnumConstant(
			const clang::EnumConstantDecl* a_decl,
			const Enum* a_enum
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
		GetEnum() const
		{
			return m_enum;
		}

		const clang::EnumConstantDecl*
		GetClangDecl() const;

	private:
		std::string m_identifier;
		int64_t m_value;

		const Enum* m_enum;
	};

	class Enum : public Type
	{
		friend class ReflectionParser;

	public:
		explicit
		Enum(const clang::EnumDecl* a_decl);

	public:
		const Type*
		GetUnderlyingType() const { return m_underlyingType; }

		std::string_view
		GetUnderlyingTypeAsString() const { return m_underlyingTypeAsString; }

		const std::vector<EnumConstant>&
		GetEntries() const { return m_entries; }

		const clang::EnumDecl*
		GetClangDecl() const;

	private:
		Type* m_underlyingType;
		std::string m_underlyingTypeAsString;
		std::vector<EnumConstant> m_entries;
	};
}
