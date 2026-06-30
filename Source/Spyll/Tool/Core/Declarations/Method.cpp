#include "Method.h"

namespace Spyll
{
	Method::Method(const clang::CXXMethodDecl* a_decl, Type* a_parent)
		: Function(a_decl, a_parent)
	{
		m_isConst = a_decl->isConst();
		m_isVirtual = a_decl->isVirtual();
	}

	Method::~Method() {}

	bool
	Method::IsConst() const
	{
		return m_isConst;
	}

	bool
	Method::IsVirtual() const
	{
		return m_isVirtual;
	}
}
