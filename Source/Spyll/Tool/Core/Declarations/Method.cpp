#include "Method.h"

namespace Spyll
{
	Method::Method(const clang::CXXMethodDecl* a_decl, Class* a_parent)
		: Function(a_decl, a_parent)
	{
		m_isConst = a_decl->isConst();
		m_isVirtual = a_decl->isVirtual();
	}

	Method::~Method() {}
}
