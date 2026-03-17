#include "pch.h"

#include "Tool.h"

#include <filesystem>

#include <clang/Tooling/CommonOptionsParser.h>
#include <llvm/Support/CommandLine.h>

namespace tooling = clang::tooling;
namespace cl = llvm::cl;

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static cl::OptionCategory MyToolCategory("my-tool options");

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
static cl::extrahelp CommonHelp(tooling::CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
static cl::extrahelp MoreHelp("\nMore help text...\n");

namespace Spyll
{
	Tool::~Tool() {}

	int
	Tool::Run()
	{
		if (m_clangTool)
		{
			m_errorCode = m_clangTool->run(tooling::newFrontendActionFactory<clang::SyntaxOnlyAction>().get());
		}
		return m_errorCode;
	}

	void
	Tool::SetPrintErrorMessage(bool a_shouldPrintMessage)
	{
		if (m_clangTool)
		{
			m_clangTool->setPrintErrorMessage(a_shouldPrintMessage);
			if (a_shouldPrintMessage)
				m_clangTool->setDiagnosticConsumer(nullptr);
			else
				m_clangTool->setDiagnosticConsumer(m_diagnosticConsumer.get());
		}
	}

	DirectTool::DirectTool(std::vector<std::string> a_fileNames, std::string a_compileArgs)
		: m_sources(std::move(a_fileNames))
	{
		std::replace(a_compileArgs.begin(), a_compileArgs.end(), ' ', '\n');
		std::string error;
		m_compilations = tooling::FixedCompilationDatabase::loadFromBuffer(
			std::filesystem::current_path().string(),
			a_compileArgs,
			error
		);

		m_clangTool = std::make_unique<tooling::ClangTool>(
			*m_compilations,
			m_sources
		);

		m_diagnosticConsumer = std::make_unique<clang::IgnoringDiagConsumer>();
		m_clangTool->setDiagnosticConsumer(m_diagnosticConsumer.get());
		m_clangTool->setPrintErrorMessage(false);
	}

	DirectTool::DirectTool(std::string a_fileName, std::string a_compileArgs)
		: DirectTool(std::vector { std::move(a_fileName) }, std::move(a_compileArgs)) {}

	DirectTool::~DirectTool() {}

	CmdTool::CmdTool(int argc, const char** argv, bool a_printErrors)
	{
		using tooling::CommonOptionsParser;
		using tooling::ClangTool;
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

	CmdTool::~CmdTool() {}
}
