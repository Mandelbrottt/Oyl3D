#pragma once

#include <clang/AST/ASTConsumer.h>

#include "ReflectionGenerator.h"
#include "Visitor.h"

namespace Spyll
{
	class DeclFinder final : public clang::ASTConsumer
	{
	public:
		explicit
		DeclFinder(ReflectionGenerator* Generator, clang::SourceManager& SM);

		void
		HandleTranslationUnit(clang::ASTContext& Ctx) override;

	private:
		DeclVisitor Visitor;
	};

	class DeclFindingAction final : public clang::ASTFrontendAction
	{
	public:
		std::unique_ptr<clang::ASTConsumer>
		CreateASTConsumer(
			clang::CompilerInstance& CI,
			llvm::StringRef InFile
		) override;

	private:
		ReflectionGenerator Generator;
	};
}
