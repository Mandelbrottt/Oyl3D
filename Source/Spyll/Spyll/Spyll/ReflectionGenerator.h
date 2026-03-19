#pragma once

namespace Spyll
{
	class ReflectionGenerator
	{
	public:
		void
		ScrapeDecl(clang::ASTContext* Context, clang::Decl* Decl);

		bool
		ShouldReflectDecl(clang::Decl* Decl) const;

	private:
		void
		ScrapeCxxRecordDecl(clang::CXXRecordDecl* Decl);

		void
		ScrapeEnumDecl(clang::EnumDecl* Decl);

		void
		ScrapeFunctionDecl(clang::FunctionDecl* Decl);
	};
}
