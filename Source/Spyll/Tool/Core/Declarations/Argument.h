#pragma once

#include "Declaration.h"
#include "Variable.h"

namespace clang
{
	class TypeDecl;
	class ParmVarDecl;
}

namespace Spyll
{
	class Type;

	class Argument : public Variable
	{
		friend class ReflectionParser;

	public:
		explicit
		Argument(const clang::ParmVarDecl* a_decl, Type* a_parent = nullptr);

		bool
		ShouldReflect() const override;

		const clang::ParmVarDecl*
		GetClangDecl() const;

	private:
		// Default argument?
	};
}
