#include "Method.h"

namespace Spyll
{
	Method::Method(const clang::CXXMethodDecl* a_decl, Type* a_parent, clang::SourceManager* a_sourceManager)
		: Function(a_decl, a_parent, a_sourceManager)
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
