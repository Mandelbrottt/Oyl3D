#include "Action.h"

#include "ReflectionGenerator.h"

namespace Spyll
{
	DeclFinder::DeclFinder(ReflectionGenerator* Generator, clang::SourceManager& SM)
		:  Visitor(Generator, SM) {}

	void
	DeclFinder::HandleTranslationUnit(clang::ASTContext& Ctx)
	{
		Visitor.SetContext(&Ctx);
		Visitor.TraverseDecl(Ctx.getTranslationUnitDecl());
	}

	std::unique_ptr<clang::ASTConsumer>
	DeclFindingAction::CreateASTConsumer(
		clang::CompilerInstance& CI,
		llvm::StringRef InFile
	)
	{
		(void) InFile;
		return std::make_unique<DeclFinder>(&Generator, CI.getSourceManager());
	}
}
