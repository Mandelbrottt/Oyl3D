#include "pch.h"
#include "ReflectionGenerator.h"

namespace Spyll
{
	void
	ReflectionGenerator::GenerateType(clang::ASTContext* Context, clang::Decl* Decl)
	{
		(void) Context;
		(void) Decl;
	}

	bool
	ReflectionGenerator::IsTypeReflected(clang::Decl* Decl) const
	{
		(void) Decl;
		return false;
	}
}
