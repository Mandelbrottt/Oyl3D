#include "Enum.h"

#include <clang/AST/QualTypeNames.h>

namespace Spyll
{
	EnumConstant::EnumConstant(
		const clang::EnumConstantDecl* a_decl,
		const Enum* a_parent,
		const clang::SourceManager* a_sourceManager
	)
		: Declaration(a_decl, a_sourceManager),
		  m_identifier(a_decl->getNameAsString()),
		  m_value(a_decl->getInitVal().getExtValue()),
		  m_parent(a_parent) {}

	EnumConstant::~EnumConstant() {}

	Enum::Enum(const clang::EnumDecl* a_decl, const clang::SourceManager* a_sourceManager)
		: Declaration(a_decl, a_sourceManager)
	{
		auto& ctx = a_decl->getASTContext();
		auto printingPolicy = ctx.getPrintingPolicy();
		auto qualifiedType = a_decl->getIntegerType();
		m_underlyingType = clang::TypeName::getFullyQualifiedName(qualifiedType, ctx, printingPolicy);

		for (auto* entryDecl : a_decl->enumerators())
		{
			m_entries.emplace_back(entryDecl, this, a_sourceManager);
		}
	}

	Enum::~Enum() {}
}
