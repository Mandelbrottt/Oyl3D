#include "Variable.h"

#include <clang/AST/QualTypeNames.h>

namespace Spyll
{
	Variable::Variable(const clang::VarDecl* a_decl, Type* a_parent, clang::SourceManager* a_sourceManager)
		: Declaration(a_decl, a_sourceManager), m_parent(a_parent)
	{
		auto& ctx = a_decl->getASTContext();
		auto printingPolicy = ctx.getPrintingPolicy();
		auto qualifiedType = a_decl->getType();
		m_type = clang::TypeName::getFullyQualifiedName(qualifiedType, ctx, printingPolicy);

		m_accessSpecifier = a_decl->getAccess();
	}

	Variable::~Variable() {}

	std::string_view
	Variable::GetTypeAsString() const
	{
		return m_type;
	}

	Type*
	Variable::GetParent() const
	{
		return m_parent;
	}

	int
	Variable::GetAccessSpecifier() const
	{
		return m_accessSpecifier;
	}
}
