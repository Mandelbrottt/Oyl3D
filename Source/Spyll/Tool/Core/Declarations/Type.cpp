#include "Type.h"

#include <clang/AST/DeclCXX.h>

namespace Spyll
{
	Type::Type(clang::CXXRecordDecl* a_decl, Type* a_parent)
		: Declaration(a_decl, a_parent)
	{
		if (a_decl->isAbstract())
		{
			m_size = 0;
			m_alignment = 0;
		} else
		{
			const auto& ctx = a_decl->getASTContext();
			const auto* type = a_decl->getTypeForDecl();
			m_size = ctx.getTypeSizeInChars(type).getQuantity();
			m_alignment = ctx.getPreferredTypeAlignInChars(type->getCanonicalTypeUnqualified()).getQuantity();
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
				// Static Member Functions are CXXMethods, but are called like Global Functions
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
				default:
				{
					break;
				}
			}
		}
	}

	bool
	Type::ShouldReflect() const
	{
		return Declaration::ShouldReflect();
	}

	const clang::CXXRecordDecl*
	Type::GetClangDecl() const
	{
		return clang::dyn_cast<clang::CXXRecordDecl>(Declaration::GetClangDecl());
	}
}
