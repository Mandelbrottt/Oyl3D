#include "Enum.h"

#include <clang/AST/QualTypeNames.h>

namespace Spyll
{
	EnumConstant::EnumConstant(
		const clang::EnumConstantDecl* a_decl,
		const Enum* a_enum
	)
		: Declaration(a_decl),
		  m_identifier(a_decl->getNameAsString()),
		  m_value(a_decl->getInitVal().getExtValue()),
		  m_enum(a_enum) {}

	EnumConstant::~EnumConstant() {}

	const clang::EnumConstantDecl*
	EnumConstant::GetClangDecl() const
	{
		return clang::dyn_cast<clang::EnumConstantDecl>(Declaration::GetClangDecl());
	}

	Enum::Enum(const clang::EnumDecl* a_decl)
		: Type(a_decl)
	{
		auto& ctx = a_decl->getASTContext();
		auto printingPolicy = ctx.getPrintingPolicy();
		auto qualifiedType = a_decl->getIntegerType();
		m_underlyingTypeAsString = clang::TypeName::getFullyQualifiedName(qualifiedType, ctx, printingPolicy);

		for (auto* entryDecl : a_decl->enumerators())
		{
			m_entries.emplace_back(entryDecl, this);
		}
	}

	const clang::EnumDecl*
	Enum::GetClangDecl() const
	{
		return clang::dyn_cast<clang::EnumDecl>(Declaration::GetClangDecl());
	}
}
