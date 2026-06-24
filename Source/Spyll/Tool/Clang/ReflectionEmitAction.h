#pragma once
#include "ReflectionParser.h"

namespace Spyll
{
	class ReflectionEmitAction final : public clang::ASTFrontendAction
	{
	public:
		explicit
		ReflectionEmitAction();

		std::unique_ptr<clang::ASTConsumer>
		CreateASTConsumer(
			clang::CompilerInstance& CI,
			llvm::StringRef InFile
		) override;	

		void EndSourceFileAction() override;

	private:
		ReflectionParser Parser;
	};
}
