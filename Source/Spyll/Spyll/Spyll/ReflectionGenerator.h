#pragma once

namespace Spyll
{
	class ReflectionGenerator
	{
	public:
		void
		GenerateType(clang::ASTContext* Context, clang::Decl* Decl);

		bool
		IsTypeReflected(clang::Decl* Decl) const;

	private:
	};
}
