#include "Type.h"

#include <clang/AST/DeclCXX.h>

namespace Spyll
{
	TemplateParam::TemplateParam(const clang::NamedDecl* a_decl)
		: Declaration(a_decl) {}

	bool
	TemplateParam::ShouldReflect() const
	{
		return Declaration::ShouldReflect();
	}

	Type::Type(const clang::TypeDecl* a_decl)
		: Declaration(a_decl)
	{
		const auto& ctx = a_decl->getASTContext();
		const auto* type = a_decl->getTypeForDecl();
		m_size = ctx.getTypeSizeInChars(type).getQuantity();
		m_alignment = ctx.getPreferredTypeAlignInChars(type->getCanonicalTypeUnqualified()).getQuantity();

		if (auto* templateParams = a_decl->getDescribedTemplateParams())
		{
			for (const auto* templateParam : *templateParams)
				m_templateParams.emplace_back(templateParam);
		}
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
