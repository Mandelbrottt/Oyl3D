#pragma once

#include "Function.h"

namespace clang
{
	class CXXMethodDecl;
}

namespace Spyll
{
	class Type;

	class Method : public Function
	{
		friend class ReflectionParser;

	public:
		explicit
		Method(const clang::CXXMethodDecl* a_decl);

		bool
		IsConst() const { return m_isConst; }

		bool
		IsVirtual() const { return m_isVirtual; }

		const clang::CXXMethodDecl*
		GetClangDecl() const;

	private:
		bool m_isConst;
		bool m_isVirtual;
	};
}
