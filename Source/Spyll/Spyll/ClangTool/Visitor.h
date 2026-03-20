#pragma once

namespace Spyll
{
	class ReflectionGenerator;
	
	class DeclVisitor final : public clang::RecursiveASTVisitor<DeclVisitor>
	{
	public:
		explicit
		DeclVisitor(ReflectionGenerator* Generator, clang::SourceManager& SM);

		bool
		TraverseDecl(clang::Decl* D);

		bool
		TraverseStmt(clang::Stmt* S);

		bool
		TraverseType(clang::QualType T);

		bool
		VisitNamedDecl(clang::NamedDecl* NamedDecl);

		clang::SourceManager*
		GetSourceManager() const;

		void
		SetContext(clang::ASTContext* Ctx);

	private:
		std::string
		GetDeclLocation(clang::SourceLocation Loc) const;

		void
		PrintDecl(const clang::NamedDecl* NamedDecl) const;

		ReflectionGenerator* Generator;

		clang::SourceManager& SourceManager;
		clang::ASTContext* Context = nullptr;
	};
}
