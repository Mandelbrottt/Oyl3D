#include "Field.h"

#include "Class.h"

#include <clang/AST/Decl.h>

namespace Spyll
{
	Field::Field(const clang::FieldDecl* a_decl, Class* a_parent)
		: Declaration(a_decl), m_parent(a_parent)
	{
		auto qualifiedType = a_decl->getType();

		m_type = qualifiedType.getLocalUnqualifiedType().getAsString();

		const auto* ctx = &a_decl->getASTContext();
		m_offsetInBits = ctx->getFieldOffset(a_decl);

		m_accessSpecifier = a_decl->getAccess();

		m_isConst = qualifiedType.isLocalConstQualified();
	}

	Field::~Field() {}
}
