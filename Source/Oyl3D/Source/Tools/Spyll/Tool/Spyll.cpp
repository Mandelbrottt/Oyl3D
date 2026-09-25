#include "Spyll/Spyll.h"

#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>

#include "Spyll/Emit.h"

using namespace clang;
namespace cl = llvm::cl;

cl::OptionCategory g_oylSpyllCategory("Oyl.Spyll options");

int
main(int argc, const char** argv)
{
	cl::opt<std::string> Assembly(
		"assembly",
		cl::desc("The name of the assembly"),
		cl::cat(g_oylSpyllCategory)
	);

	cl::list<std::string> Dependencies(
		"dependency",
		cl::desc("The name of the assembly"),
		cl::cat(g_oylSpyllCategory),
		cl::ZeroOrMore
	);

	auto ExpectedParser = tooling::CommonOptionsParser::create(argc, argv, g_oylSpyllCategory);
	if (!ExpectedParser)
	{
		llvm::errs() << ExpectedParser.takeError();
		return 1;
	}

	tooling::CommonOptionsParser& OptionsParser = ExpectedParser.get();

	std::string virtualGenFilePath = "__gen.cpp";
	std::string tempContents;
	for (const auto& path : OptionsParser.getSourcePathList())
	{
		tempContents += "#include <" + path + ">\n";
	}

	tooling::ClangTool Tool { OptionsParser.getCompilations(), virtualGenFilePath };
	Tool.mapVirtualFile(virtualGenFilePath, tempContents);
	Tool.appendArgumentsAdjuster(tooling::getInsertArgumentAdjuster("-Wno-everything"));
	Tool.appendArgumentsAdjuster(tooling::getInsertArgumentAdjuster("-D__REFLECT_GENERATE__=1"));
	Tool.appendArgumentsAdjuster(tooling::getClangSyntaxOnlyAdjuster());
	Tool.appendArgumentsAdjuster(tooling::getClangStripOutputAdjuster());

	Spyll::ReflectionContext ReflectionContext;
	ReflectionContext.assemblyName = Assembly;
	ReflectionContext.dependencies = Dependencies;
	Spyll::ReflectionActionFactory Factory(&ReflectionContext, EmitCodeFromTool);

	Tool.setPrintErrorMessage(false);
	Tool.run(&Factory);
}
