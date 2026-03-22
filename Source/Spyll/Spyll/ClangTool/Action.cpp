#include "Action.h"

#include "ReflectionGenerator.h"
#include "SpyllTool.h"

namespace Spyll
{
	DeclFinder::DeclFinder(clang::SourceManager& SM, ReflectionGenerator* Generator)
		: Visitor(SM, Generator) {}

	void
	DeclFinder::HandleTranslationUnit(clang::ASTContext& Ctx)
	{
		Visitor.SetContext(&Ctx);
		Visitor.TraverseDecl(Ctx.getTranslationUnitDecl());
	}

	DeclFindingAction::DeclFindingAction(SpyllTool* Tool)
		: Tool(Tool) {}

	std::unique_ptr<clang::ASTConsumer>
	DeclFindingAction::CreateASTConsumer(
		clang::CompilerInstance& CI,
		llvm::StringRef InFile
	)
	{
		(void) InFile;
		auto* generator = Tool->CreateNewReflectionGenerator(InFile);
		return std::make_unique<DeclFinder>(CI.getSourceManager(), generator);
	}
}
