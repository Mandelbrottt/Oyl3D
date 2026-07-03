#include "Field.h"

#include "Type.h"

#include <clang/AST/Decl.h>
#include <clang/AST/QualTypeNames.h>

namespace Spyll
{
	Field::Field(const clang::FieldDecl* a_decl, Type* a_parent)
		: Declaration(a_decl), m_parent(a_parent)
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

	bool
	Field::ShouldReflect() const
	{
		auto fieldDecl = clang::dyn_cast<clang::FieldDecl>(m_decl);

		bool result = [&, this]
		{
			auto type = fieldDecl->getType().getTypePtr();
			while (type->isPointerType())
			{
				type = type->getPointeeOrArrayElementType();
			}
			auto typeDecl = type->getAsRecordDecl();
			if (typeDecl
				&& (typeDecl->getAccess() == clang::AS_protected
					|| typeDecl->getAccess() == clang::AS_private))
			{
				return false;
			}

			return true;
		}();

		return result && Declaration::ShouldReflect();
	}

	std::string_view
	Field::GetTypeAsString() const
	{
		return m_type;
	}

	Type*
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
