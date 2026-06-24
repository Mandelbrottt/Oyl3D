#pragma once

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
		std::string type;
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

		std::string m_returnType;

		std::vector<Argument> m_arguments;
	};
}
