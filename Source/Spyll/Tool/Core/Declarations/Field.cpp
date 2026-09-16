#include "Field.h"

#include "Type.h"

#include <clang/AST/Decl.h>
#include <clang/AST/QualTypeNames.h>

namespace Spyll
{
	Field::Field(const clang::FieldDecl* a_decl, Type* a_parentType)
		: Declaration(a_decl, a_parentType)
	{
		auto& ctx = a_decl->getASTContext();
		m_offsetInBits = ctx.getFieldOffset(a_decl);

		auto qualifiedType = a_decl->getType();
		m_isConst = qualifiedType.isLocalConstQualified();

		auto printingPolicy = ctx.getPrintingPolicy();
		m_typeAsString = clang::TypeName::getFullyQualifiedName(qualifiedType, ctx, printingPolicy);
	}

	bool
	Field::ShouldReflect() const
	{
		if (!Declaration::ShouldReflect())
			return false;

		//auto fieldDecl = GetClangDecl();

		//if (!IsTypeOfDeclVisible(fieldDecl))
		//	return false;

		return true;
	}

	const clang::FieldDecl*
	Field::GetClangDecl() const
	{
		return clang::dyn_cast<clang::FieldDecl>(Declaration::GetClangDecl());
	}
}
