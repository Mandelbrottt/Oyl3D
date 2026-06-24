#pragma once

#include "Declaration.h"

namespace clang
{
	class VarDecl;
}

namespace Spyll
{
	class Class;

	class Variable : public Declaration
	{
	public:
		explicit
		Variable(const clang::VarDecl* a_decl, Class* a_parent = nullptr);

		virtual
		~Variable();

	private:
		clang::QualType m_type;

		Class* m_parent;
	};
}
