#pragma once

#include <clang/Frontend/FrontendAction.h>

#include "ReflectionParser.h"

#include "ReflectionParserOptions.h"

namespace Spyll
{
	class ReflectionAction_ final : public clang::ASTFrontendAction
	{
	public:
		explicit
		ReflectionAction_(ReflectionParserOptions* a_options);

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
