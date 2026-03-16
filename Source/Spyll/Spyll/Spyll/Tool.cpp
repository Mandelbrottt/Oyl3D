#include "pch.h"

#include "Tool.h"

#include <filesystem>

#include <clang/Tooling/CommonOptionsParser.h>
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
	Tool::Tool(std::vector<std::string> a_fileNames, std::string a_compileArgs)
		: m_sources(std::move(a_fileNames))
	{
		std::replace(a_compileArgs.begin(), a_compileArgs.end(), ' ', '\n');
		std::string error;
		m_compilations = FixedCompilationDatabase::loadFromBuffer(
			std::filesystem::current_path().string(),
			a_compileArgs,
			error
		);

		m_clangTool = std::make_unique<ClangTool>(
			*m_compilations,
			a_fileNames
		);
	}

	Tool::Tool(std::string a_fileName, std::string a_compileArgs)
		: Tool(std::vector { std::move(a_fileName) }, std::move(a_compileArgs)) {}

	Tool::Tool(size_t argc, const char** argv)
	{
		int int_argc = (int) argc;
		auto ExpectedOptions = CommonOptionsParser::create(int_argc, argv, MyToolCategory);
		if (!ExpectedOptions)
		{
			llvm::errs() << ExpectedOptions.takeError();
			return;
		}
		m_clangTool = std::make_unique<ClangTool>(
			ExpectedOptions->getCompilations(),
			ExpectedOptions->getSourcePathList()
		);
	}

	Tool::~Tool() {}

	int
	Tool::Run()
	{
		if (m_clangTool)
		{
			m_errorCode = m_clangTool->run(newFrontendActionFactory<clang::SyntaxOnlyAction>().get());
		}
		return m_errorCode;
	}
}
