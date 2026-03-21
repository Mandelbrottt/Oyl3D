#include "Action.h"

#include "ReflectionGenerator.h"

namespace Spyll
{
	DeclFinder::DeclFinder(clang::SourceManager& SM)
		:  Visitor(SM) {}

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
		return std::make_unique<DeclFinder>(CI.getSourceManager());
	}
}
