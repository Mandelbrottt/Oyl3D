#include "Enum.h"

#include <clang/AST/QualTypeNames.h>

namespace Spyll
{
	Enum::Enum(const clang::EnumDecl* a_decl, clang::SourceManager* a_sourceManager)
		: Declaration(a_decl, a_sourceManager)
	{
		auto& ctx = a_decl->getASTContext();
		auto printingPolicy = ctx.getPrintingPolicy();
		auto qualifiedType = a_decl->getIntegerType();
		m_underlyingType = clang::TypeName::getFullyQualifiedName(qualifiedType, ctx, printingPolicy);

		for (auto* entryDecl : a_decl->enumerators())
		{
			m_entries.emplace_back(Entry {
				entryDecl->getNameAsString(),
				entryDecl->getInitVal().getExtValue()
			});
		}
	}

	Enum::~Enum() {}
}
