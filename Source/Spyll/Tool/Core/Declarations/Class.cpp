#include "Class.h"

namespace Spyll
{
	Class::Class(clang::CXXRecordDecl* a_decl)
		: Record(a_decl)
	{
		if (a_decl->isAbstract())
		{
			m_size = 0;
			m_alignment = 0;
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
				// Static Member Functions are CXXMethods, but are called like Global Functions
				case clang::Decl::CXXMethod:
				{
					auto* cxxMethodDecl = static_cast<clang::CXXMethodDecl*>(decl);
					if (cxxMethodDecl->isInstance())
						m_methods.emplace_back(cxxMethodDecl);
					else
						m_functions.emplace_back(cxxMethodDecl);
					break;
				}
				case clang::Decl::Var:
				{
					auto* varDecl = static_cast<clang::VarDecl*>(decl);
					m_variables.emplace_back(varDecl);
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
	Class::ShouldReflect() const
	{
		return Declaration::ShouldReflect();
	}

	const clang::CXXRecordDecl*
	Class::GetClangDecl() const
	{
		return clang::dyn_cast<clang::CXXRecordDecl>(Type::GetClangDecl());
	}
}
