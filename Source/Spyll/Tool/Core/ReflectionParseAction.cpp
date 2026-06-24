#include "ReflectionParseAction.h"

namespace Spyll
{
	ReflectionParseAction::ReflectionParseAction(ReflectionParseOptions* a_options)
		: m_options(a_options) {}

	std::unique_ptr<clang::ASTConsumer>
	ReflectionParseAction::CreateASTConsumer(
		clang::CompilerInstance& CI,
		llvm::StringRef InFile
	)
	{
		(void) InFile;

		CI.getDiagnosticOpts().IgnoreWarnings = true;

		Parser.SetDiagnosticOptions(&CI.getDiagnosticOpts());

		return std::make_unique<ReflectionParserConsumer>(CI.getSourceManager(), &Parser);
	}

	void
	ReflectionParseAction::EndSourceFileAction()
	{
		if (m_options->onSourceParsedCallback)
		{
			auto onSourceParsedCallback = m_options->onSourceParsedCallback;
			onSourceParsedCallback(&Parser);
		}
	}

	ReflectionParserConsumer::ReflectionParserConsumer(clang::SourceManager& SM, ReflectionParser* Parser)
		: Parser(Parser)
	{
		Parser->SetSourceManager(&SM);
	}

	void
	ReflectionParserConsumer::HandleTranslationUnit(clang::ASTContext& Ctx)
	{
		Parser->SetContext(&Ctx);
		Parser->TraverseDecl(Ctx.getTranslationUnitDecl());
	}
}
