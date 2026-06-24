#include "Class.h"

#include <clang/AST/DeclCXX.h>

namespace Spyll
{
	Class::Class(clang::CXXRecordDecl* a_decl)
		: Declaration(a_decl)
	{
		auto* type = a_decl->getTypeForDecl();

		const auto& ctx = a_decl->getASTContext();
		m_size = ctx.getTypeSizeInChars(type).getQuantity();
		m_alignment = ctx.getPreferredTypeAlignInChars(type->getCanonicalTypeUnqualified()).getQuantity();

		for (const auto& decl : a_decl->bases())
		{
			m_baseClasses.emplace_back(BaseClass { decl.getType().getAsString() });
		}

		for (auto* decl : a_decl->decls())
		{
			switch (decl->getKind())
			{
				case clang::Decl::CXXConstructor:
				{
					break;
				}
				case clang::Decl::CXXDestructor:
				{
					break;
				}
				case clang::Decl::Field:
				{
					auto* fieldDecl = static_cast<clang::FieldDecl*>(decl);
					m_fields.emplace_back(fieldDecl, this);
					break;
				}
				case clang::Decl::CXXMethod:
				{
					auto* cxxMethodDecl = static_cast<clang::CXXMethodDecl*>(decl);
					if (cxxMethodDecl->isInstance())
						m_methods.emplace_back(cxxMethodDecl, this);
					else
						m_functions.emplace_back(cxxMethodDecl, this);
					break;
				}
				case clang::Decl::Var:
				{
					auto* varDecl = static_cast<clang::VarDecl*>(decl);
					m_variables.emplace_back(varDecl, this);
					break;
				}
			}
		}
	}

	Class::~Class() {}

	bool
	Class::ShouldReflect() const
	{
		return Declaration::ShouldReflect();
	}
}
