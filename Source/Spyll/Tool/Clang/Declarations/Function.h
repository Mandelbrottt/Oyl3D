#pragma once

#include <clang/AST/Type.h>

#include "Declaration.h"

namespace clang
{
	class FunctionDecl;
}

namespace Spyll
{
	class Class;

	struct Argument
	{
		clang::QualType type;
		std::string name;
	};

	class Function : public Declaration
	{
	public:
		explicit
		Function(const clang::FunctionDecl* a_decl, Class* a_parent = nullptr);

		virtual
		~Function();

	private:
		Class* m_parent;

		clang::QualType m_returnType;

		std::vector<Argument> m_arguments;
	};
}
