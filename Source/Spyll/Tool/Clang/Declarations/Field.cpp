#include "Field.h"

#include "Class.h"

#include <clang/AST/Decl.h>

namespace Spyll
{
	Field::Field(const clang::FieldDecl* a_decl, Class* a_parent)
		: Declaration(a_decl), m_parent(a_parent)
	{
		m_type = a_decl->getType().getCanonicalType().getAsString();

		const auto* ctx = &a_decl->getASTContext();
		m_offsetInBits = ctx->getFieldOffset(a_decl);

		m_accessSpecifier = a_decl->getAccess();

		m_isConst = a_decl->getType().isConstQualified();
	}

	Field::~Field() {}
}
