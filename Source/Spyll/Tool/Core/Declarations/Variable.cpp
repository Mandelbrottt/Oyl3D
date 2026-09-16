#include "Variable.h"

#include <clang/AST/QualTypeNames.h>

namespace Spyll
{
	Variable::Variable(const clang::VarDecl* a_decl, Type* a_parent)
		: Declaration(a_decl, a_parent)
	{
		auto& ctx = a_decl->getASTContext();
		auto qualifiedType = a_decl->getType();
		auto printingPolicy = ctx.getPrintingPolicy();
		m_typeAsString = clang::TypeName::getFullyQualifiedName(qualifiedType, ctx, printingPolicy);
	}

	bool
	Variable::ShouldReflect() const
	{
		if (!Declaration::ShouldReflect())
		{
			return false;
		}

		auto varDecl = GetClangDecl();

		return !varDecl->isConstexpr();
	}

	const clang::VarDecl*
	Variable::GetClangDecl() const
	{
		return clang::dyn_cast<clang::VarDecl>(Declaration::GetClangDecl());
	}
}
