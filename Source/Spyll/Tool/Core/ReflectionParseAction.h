#pragma once

#include <clang/Frontend/FrontendAction.h>

#include "ReflectionParser.h"

#include "ReflectionParserOptions.h"

namespace Spyll
{
	class ReflectionParseAction final : public clang::ASTFrontendAction
	{
	public:
		explicit
		ReflectionParseAction(ReflectionParserOptions* a_options);

		std::unique_ptr<clang::ASTConsumer>
		CreateASTConsumer(
			clang::CompilerInstance& CI,
			llvm::StringRef InFile
		) override;	

		void EndSourceFileAction() override;

	private:
		ReflectionParserOptions* m_options;

		ReflectionParser Parser;
	};
}
