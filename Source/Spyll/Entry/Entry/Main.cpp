#include "Spyll/Tool.h"

#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/CompilationDatabase.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/CommandLine.h>

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static llvm::cl::OptionCategory MyToolCategory("oyl-spyll options");

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
static llvm::cl::extrahelp CommonHelp(clang::tooling::CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
static llvm::cl::extrahelp MoreHelp("\nMore help text...\n");

class CmdTool final : public Spyll::Tool
{
public:
	explicit
	CmdTool(int argc, const char** argv, bool a_printErrors = true)
	{
		using clang::tooling::CommonOptionsParser;
		using clang::tooling::ClangTool;
		
		auto ExpectedOptions = CommonOptionsParser::create(argc, argv, MyToolCategory);
		if (!ExpectedOptions)
		{
			if (a_printErrors)
				llvm::errs() << ExpectedOptions.takeError();
			return;
		}

		CommonOptionsParser& OptionsParser = ExpectedOptions.get();
		m_optionsParser = std::make_unique<CommonOptionsParser>(std::move(OptionsParser));
		m_clangTool = std::make_unique<ClangTool>(
			m_optionsParser->getCompilations(),
			m_optionsParser->getSourcePathList()
		);

		SetPrintErrorMessage(a_printErrors);
	}

private:
	std::unique_ptr<clang::tooling::CommonOptionsParser> m_optionsParser;
};

int
main(int argc, const char** argv)
{
	CmdTool tool(argc, argv);
	return tool.Run();
}
