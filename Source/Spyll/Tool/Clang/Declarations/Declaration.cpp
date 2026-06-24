#include "Declaration.h"

#include <clang/AST/Decl.h>

namespace Spyll
{
	Declaration::Declaration(const clang::NamedDecl* a_decl)
	{
		m_enabled = true;
		m_sourceLocationEncoding = a_decl->getLocation().getPtrEncoding();
	}

	Declaration::~Declaration() {}

	bool
	Declaration::ShouldReflect() const
	{
		return m_enabled;
	}

	void*
	Declaration::GetSourceLocationEncoding() const
	{
		return m_sourceLocationEncoding;
	}
}
