#include "Argument.h"

namespace Spyll
{
	Argument::Argument(const clang::ParmVarDecl* a_decl, Type* a_parent)
		: Variable(a_decl, a_parent) {}

	bool
	Argument::ShouldReflect() const
	{
		if (!Variable::ShouldReflect())
		{
			return false;
		}

		return true;
	}

	const clang::ParmVarDecl*
	Argument::GetClangDecl() const
	{
		return clang::dyn_cast<clang::ParmVarDecl>(Variable::GetClangDecl());
	}
}
