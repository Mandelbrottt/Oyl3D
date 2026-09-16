#include "Method.h"

namespace Spyll
{
	Method::Method(const clang::CXXMethodDecl* a_decl, Type* a_parent)
		: Function(a_decl, a_parent)
	{
		m_isConst = a_decl->isConst();
		m_isVirtual = a_decl->isVirtual();
	}

	const clang::CXXMethodDecl*
	Method::GetClangDecl() const
	{
		return clang::dyn_cast<clang::CXXMethodDecl>(Function::GetClangDecl());
	}
}
