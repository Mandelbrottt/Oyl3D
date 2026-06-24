#pragma once

#include "Function.h"

namespace clang
{
	class CXXMethodDecl;
}

namespace Spyll
{
	class Class;

	class Method : public Function
	{
	public:
		explicit
		Method(const clang::CXXMethodDecl* a_decl, Class* a_parent, clang::SourceManager* a_sourceManager);

		virtual
		~Method();

		bool
		IsConst() const;

		bool
		IsVirtual() const;

	private:
		bool m_isConst;
		bool m_isVirtual;
	};
}
