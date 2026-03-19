#include "Main.h"

namespace Spyll
{
	TestTool::TestTool(std::vector<std::string> a_fileNames, std::string a_compileArgs)
		: m_sources(std::move(a_fileNames))
	{
		std::replace(a_compileArgs.begin(), a_compileArgs.end(), ' ', '\n');
		std::string error;
		m_compilations = clang::tooling::FixedCompilationDatabase::loadFromBuffer(
			std::filesystem::current_path().string(),
			a_compileArgs,
			error
		);

		m_clangTool = std::make_unique<clang::tooling::ClangTool>(
			*m_compilations,
			m_sources
		);

		// Disable Error Output from the clang tool
		m_diagnosticConsumer = std::make_unique<clang::IgnoringDiagConsumer>();
		SetPrintErrorMessage(false);
	}

	TestTool::TestTool(std::string a_fileName, std::string a_compileArgs)
		: TestTool(std::vector { std::move(a_fileName) }, std::move(a_compileArgs)) {}

	TestTool::~TestTool() {}
}

int
main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
