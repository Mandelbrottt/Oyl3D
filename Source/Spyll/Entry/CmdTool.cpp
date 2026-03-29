#include "CmdTool.h"

#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/CompilationDatabase.h>
#include <clang/Tooling/Tooling.h>

#include <llvm/Support/CommandLine.h>

#include "Spyll/ClangTool/SpyllTool.h"

namespace cl = llvm::cl;

cl::OptionCategory g_spyllToolCategory("oyl-spyll options");

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
static cl::extrahelp CommonHelp(clang::tooling::CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
static cl::extrahelp MoreHelp("\nMore help text...\n");

namespace Spyll
{
	struct CmdTool::Impl
	{
		std::unique_ptr<clang::tooling::CompilationDatabase> compilationDatabase;
		std::vector<std::string> sourcePaths;

		cl::NumOccurrencesFlag occurrencesFlag;
	};

	CmdTool::CmdTool(int argc, const char** argv, bool a_printErrors)
		: m_impl(std::make_unique<Impl>())
	{
		auto err = Init(argc, argv);
		if (!err) {}

		m_clangTool = std::make_unique<clang::tooling::ClangTool>(
			*m_impl->compilationDatabase,
			m_impl->sourcePaths
		);

		SetPrintErrorMessage(a_printErrors);
	}

	CmdTool::~CmdTool() {}

	llvm::Error
	CmdTool::Init(int argc, const char** argv)
	{
		static cl::opt<std::string> BuildPath(
			"p",
			cl::desc("Build path"),
			cl::Optional,
			cl::cat(g_spyllToolCategory),
			cl::sub(cl::SubCommand::getAll())
		);

		static cl::list<std::string> SourcePaths(
			cl::Positional,
			cl::desc("<source0> [... <sourceN>]"),
			cl::NumOccurrencesFlag::OneOrMore,
			cl::cat(g_spyllToolCategory),
			cl::sub(cl::SubCommand::getAll())
		);

		m_impl->occurrencesFlag = cl::NumOccurrencesFlag::OneOrMore;

		cl::ResetAllOptionOccurrences();

		cl::HideUnrelatedOptions(g_spyllToolCategory);

		std::string ErrorMessage;
		m_impl->compilationDatabase =
			clang::tooling::FixedCompilationDatabase::loadFromCommandLine(argc, argv, ErrorMessage);

		if (!ErrorMessage.empty())
			ErrorMessage.append("\n");
		// Stop initializing if command-line option parsing failed.
		if (!cl::ParseCommandLineOptions(argc, argv))
		{
			return llvm::make_error<llvm::StringError>(
				ErrorMessage,
				llvm::inconvertibleErrorCode()
			);
		}

		cl::PrintOptionValues();

		m_impl->sourcePaths = SourcePaths;
		if ((m_impl->occurrencesFlag == cl::ZeroOrMore || m_impl->occurrencesFlag == cl::Optional) &&
			m_impl->sourcePaths.empty())
			return llvm::Error::success();
		if (!m_impl->compilationDatabase)
		{
			if (!BuildPath.empty())
			{
				m_impl->compilationDatabase =
					clang::tooling::CompilationDatabase::autoDetectFromDirectory(BuildPath, ErrorMessage);
			} else
			{
				m_impl->compilationDatabase = clang::tooling::CompilationDatabase::autoDetectFromSource(
					SourcePaths[0],
					ErrorMessage
				);
			}
			if (!m_impl->compilationDatabase)
			{
				llvm::errs() << "Error while trying to load a compilation database:\n"
					<< ErrorMessage << "Running without flags.\n";
				m_impl->compilationDatabase.reset(
					new clang::tooling::FixedCompilationDatabase(".", std::vector<std::string>())
				);
			}
		}
		return llvm::Error::success();
	}
}
