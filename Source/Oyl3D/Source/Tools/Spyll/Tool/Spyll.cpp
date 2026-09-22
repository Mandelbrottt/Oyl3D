#include "Spyll/Spyll.h"

#include <clang/Basic/DiagnosticOptions.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>

using namespace clang;
namespace cl = llvm::cl;

cl::OptionCategory g_oylSpyllCategory("Oyl.Spyll options");

int
main(int argc, const char** argv)
{
	auto ExpectedParser = tooling::CommonOptionsParser::create(argc, argv, g_oylSpyllCategory);
	if (!ExpectedParser)
	{
		llvm::errs() << ExpectedParser.takeError();
		return 1;
	}

	tooling::CommonOptionsParser& OptionsParser = ExpectedParser.get();

	std::string tempPath = "__gen.cpp";
	std::string tempContents;
	for (const auto& path : OptionsParser.getSourcePathList())
	{
		tempContents += "#include <" + path + ">\n";
	}

	tooling::ClangTool Tool { OptionsParser.getCompilations(), tempPath };
	Tool.mapVirtualFile(tempPath, tempContents);

	Tool.appendArgumentsAdjuster(tooling::getClangSyntaxOnlyAdjuster());
	Tool.appendArgumentsAdjuster(tooling::getClangStripOutputAdjuster());

	Spyll::ReflectionContext ReflectionContext;

	Spyll::ReflectionActionFactory Factory(ReflectionContext);

	Tool.setPrintErrorMessage(false);
	Tool.run(&Factory);
}
