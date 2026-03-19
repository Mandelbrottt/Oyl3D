#include "pch.h"
#include "Visitor.h"

#include "ReflectionGenerator.h"

namespace Spyll
{
	DeclVisitor::DeclVisitor(ReflectionGenerator* Generator, clang::SourceManager& SM)
		: Generator(Generator), SourceManager(SM) {}

	bool
	DeclVisitor::TraverseDecl(clang::Decl* D)
	{
		if (!D)
		{
			return true;
		}

		Generator->ScrapeDecl(Context, D);

		if (Generator->ShouldReflectDecl(D))
		{
			auto RD = clang::dyn_cast<clang::RecordDecl>(D);
			PrintDecl(RD);
		}

		return RecursiveASTVisitor::TraverseDecl(D);
	}

	bool
	DeclVisitor::TraverseStmt(clang::Stmt* S)
	{
		return RecursiveASTVisitor::TraverseStmt(S);
	}

	bool
	DeclVisitor::TraverseType(clang::QualType T)
	{
		return RecursiveASTVisitor::TraverseType(T);
	}

	bool
	DeclVisitor::VisitNamedDecl(clang::NamedDecl* NamedDecl)
	{
		return RecursiveASTVisitor::VisitNamedDecl(NamedDecl);
	}

	clang::SourceManager*
	DeclVisitor::GetSourceManager() const
	{
		return &SourceManager;
	}

	void
	DeclVisitor::SetContext(clang::ASTContext* Ctx)
	{
		Context = Ctx;
	}

	std::string
	DeclVisitor::GetDeclLocation(clang::SourceLocation Loc) const
	{
		return Loc.printToString(SourceManager);
	}

	void
	DeclVisitor::PrintDecl(const clang::NamedDecl* NamedDecl) const
	{
		(void) NamedDecl;	
	}
}
