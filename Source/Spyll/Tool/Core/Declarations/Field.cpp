#include "Field.h"

#include "Class.h"

#include <clang/AST/Decl.h>
#include <clang/AST/QualTypeNames.h>

namespace Spyll
{
	Field::Field(const clang::FieldDecl* a_decl, Class* a_parent, clang::SourceManager* a_sourceManager)
		: Declaration(a_decl, a_sourceManager), m_parent(a_parent)
	{
		auto& ctx = a_decl->getASTContext();
		auto printingPolicy = ctx.getPrintingPolicy();
		auto qualifiedType = a_decl->getType();
		m_type = clang::TypeName::getFullyQualifiedName(qualifiedType, ctx, printingPolicy);

		m_offsetInBits = ctx.getFieldOffset(a_decl);

		m_accessSpecifier = a_decl->getAccess();

		m_isConst = qualifiedType.isLocalConstQualified();
	}

	Field::~Field() {}

	std::string_view
	Field::GetTypeAsString() const
	{
		return m_type;
	}

	Class*
	Field::GetParent() const
	{
		return m_parent;
	}

	size_t
	Field::GetOffsetInBits() const
	{
		return m_offsetInBits;
	}

	int
	Field::GetAccessSpecifier() const
	{
		return m_accessSpecifier;
	}

	bool
	Field::IsConst() const
	{
		return m_isConst;
	}
}
