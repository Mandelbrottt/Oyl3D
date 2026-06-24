#include "Function.h"

namespace Spyll
{
	Function::Function(const clang::FunctionDecl* a_decl, Class* a_parent)
		: Declaration(a_decl), m_parent(a_parent)
	{
		m_returnType = a_decl->getReturnType();

		for (auto* argDecl : a_decl->parameters())
		{
			m_arguments.emplace_back(Argument {
				argDecl->getType(),
				argDecl->getNameAsString()
			});
		}
	}

	Function::~Function() {}
}
