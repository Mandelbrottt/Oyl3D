#include "Type.h"

#include <clang/AST/Decl.h>

namespace Spyll
{
	Type::Type(const clang::Type* a_type)
		: Declaration(a_type->getAsTagDecl())
	{
		const auto& ctx = a_type->getAsTagDecl()->getASTContext();
		m_size = ctx.getTypeSizeInChars(a_type).getQuantity();
		m_alignment = ctx.getPreferredTypeAlignInChars(a_type->getCanonicalTypeUnqualified()).getQuantity();
	}

	Type::~Type() {}
}
