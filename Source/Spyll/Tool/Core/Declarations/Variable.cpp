#include "Variable.h"

namespace Spyll
{
	Variable::Variable(const clang::VarDecl* a_decl, Class* a_parent)
		: Declaration(a_decl), m_parent(a_parent)
	{
		m_type = a_decl->getType().getLocalUnqualifiedType().getAsString();
	}

	Variable::~Variable() {}
}
