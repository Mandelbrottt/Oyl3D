#pragma once

#include <clang/AST/Type.h>

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
		Field(const clang::FieldDecl* a_decl, Class* a_parent);

		virtual
		~Field();

	private:
		clang::QualType m_type;

		Class* m_parent;

		size_t m_offsetInBits;

		int m_accessSpecifier;

		bool m_isConst;
	};
}
