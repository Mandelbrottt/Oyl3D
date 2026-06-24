#include "ReflectionEmitAction.h"

#include "ReflectionEmitter.h"

namespace Spyll
{
	ReflectionEmitAction::ReflectionEmitAction() {}

	std::unique_ptr<clang::ASTConsumer>
	ReflectionEmitAction::CreateASTConsumer(
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
	ReflectionEmitAction::EndSourceFileAction()
	{
		ReflectionEmitter emitter;

		emitter.Compile(&Parser);

		emitter.Emit();
	}
}
