#include "Tool.h"

#include <filesystem>

#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/CommandLine.h>

using namespace clang::tooling;
using namespace llvm;

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static cl::OptionCategory MyToolCategory("my-tool options");

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
static cl::extrahelp MoreHelp("\nMore help text...\n");

namespace Spyll
{
	Tool::Tool(const std::string& a_fileName, const std::string& a_compileArgs)
		: m_file(a_fileName), m_compileArgs(a_compileArgs)
	{
		std::replace(m_compileArgs.begin(), m_compileArgs.end(), ' ', '\n');
	}

	int
	Tool::Run()
	{
		auto database = FixedCompilationDatabase::loadFromBuffer(
			std::filesystem::current_path().string(),
			m_compileArgs,
			m_databaseError
		);
		
		auto tool = ClangTool(
			*database,
			std::vector { m_file }
		);
		m_errorCode = tool.run(newFrontendActionFactory<clang::SyntaxOnlyAction>().get());
		return m_errorCode;
	}
}
