#pragma once

#include "Declaration.h"

namespace clang
{
	class FieldDecl;
}

namespace Spyll
{
	class Class;

	class Field : public Declaration
	{
	public:
		explicit
		Field(const clang::FieldDecl* a_decl, Class* a_parent, clang::SourceManager* a_sourceManager);

		virtual
		~Field();

		std::string_view
		GetTypeAsString() const;

		Class*
		GetParent() const;

		size_t
		GetOffsetInBits() const;

		int
		GetAccessSpecifier() const;

		bool
		IsConst() const;

	private:
		std::string m_type;

		Class* m_parent;

		size_t m_offsetInBits;

		int m_accessSpecifier;

		bool m_isConst;
	};
}
