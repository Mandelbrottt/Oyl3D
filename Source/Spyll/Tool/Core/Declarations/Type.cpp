#include "Type.h"

#include <clang/AST/DeclCXX.h>

namespace Spyll
{
	Type::Type(const clang::TypeDecl* a_decl)
		: Declaration(a_decl)
	{
		const auto& ctx = a_decl->getASTContext();
		const auto* type = a_decl->getTypeForDecl();
		m_size = ctx.getTypeSizeInChars(type).getQuantity();
		m_alignment = ctx.getPreferredTypeAlignInChars(type->getCanonicalTypeUnqualified()).getQuantity();
	}

	bool
	Type::ShouldReflect() const
	{
		return Declaration::ShouldReflect();
	}

	const clang::TypeDecl*
	Type::GetClangDecl() const
	{
		return clang::dyn_cast<clang::TypeDecl>(Declaration::GetClangDecl());
	}
}
