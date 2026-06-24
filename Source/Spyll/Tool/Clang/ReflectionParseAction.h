#pragma once

#include "ReflectionParser.h"

namespace Spyll
{
	using OnSourceParsedFn = void(*)(const ReflectionParser*);

	struct ReflectionEmitOptions
	{
		OnSourceParsedFn onSourceParsedCallback;
	};

	class ReflectionParseAction final : public clang::ASTFrontendAction
	{
	public:
		explicit
		ReflectionParseAction(ReflectionEmitOptions&& a_options);

		std::unique_ptr<clang::ASTConsumer>
		CreateASTConsumer(
			clang::CompilerInstance& CI,
			llvm::StringRef InFile
		) override;	

		void EndSourceFileAction() override;

	private:
		ReflectionEmitOptions m_options;

		ReflectionParser Parser;
	};

	class ReflectionParserConsumer final : public clang::ASTConsumer
	{
	public:
		explicit
		ReflectionParserConsumer(clang::SourceManager& SM, ReflectionParser* Parser);

		void
		HandleTranslationUnit(clang::ASTContext& Ctx) override;

	private:
		ReflectionParser* Parser;
	};
}
