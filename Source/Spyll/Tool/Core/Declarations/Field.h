#pragma once

#include "Declaration.h"

namespace clang
{
	class FieldDecl;
}

namespace Spyll
{
	class Type;

	class Field : public Declaration
	{
	public:
		explicit
		Field(const clang::FieldDecl* a_decl, Type* a_parent, const clang::SourceManager* a_sourceManager);

		virtual
		~Field();

		std::string_view
		GetTypeAsString() const;

		Type*
		GetParent() const;

		size_t
		GetOffsetInBits() const;

		int
		GetAccessSpecifier() const;

		bool
		IsConst() const;

	private:
		std::string m_type;

		Type* m_parent;

		size_t m_offsetInBits;

		int m_accessSpecifier;

		bool m_isConst;
	};
}
