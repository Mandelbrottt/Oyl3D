#pragma once

#include <clang/AST/ASTConsumer.h>
#include <clang/Frontend/FrontendAction.h>

#include "Spyll/ClangTool/ReflectionGenerator.h"
#include "Spyll/ClangTool/Visitor.h"

namespace Spyll
{
	class SpyllTool;

	class DeclFinder final : public clang::ASTConsumer
	{
	public:
		explicit
		DeclFinder(clang::SourceManager& SM, ReflectionGenerator* Generator);

		void
		HandleTranslationUnit(clang::ASTContext& Ctx) override;

	private:
		DeclVisitor Visitor;
	};

	class DeclFindingAction final : public clang::ASTFrontendAction
	{
	public:
		explicit
		DeclFindingAction(SpyllTool* Tool);

		std::unique_ptr<clang::ASTConsumer>
		CreateASTConsumer(
			clang::CompilerInstance& CI,
			llvm::StringRef InFile
		) override;

	private:
		SpyllTool* Tool = nullptr;
	};
}
