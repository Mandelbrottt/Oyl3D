#include "Main.h"

namespace Spyll
{
	class TestDiagnosticConsumer : public clang::DiagnosticConsumer
	{
	public:
		void
		HandleDiagnostic(
			clang::DiagnosticsEngine::Level DiagLevel, 
			const clang::Diagnostic& Info
		) override
		{
			(void) Info;
			if (DiagLevel == clang::DiagnosticsEngine::Error)
			{
				NumErrors++;
			}
		}
	};

	TestTool::TestTool(std::vector<std::string> a_fileNames, std::string a_compileArgs)
	{
		Setup(std::move(a_fileNames), std::move(a_compileArgs));
	}

	TestTool::TestTool(std::string a_fileName, std::string a_compileArgs)
	{
		Setup(std::move(a_fileName), std::move(a_compileArgs));
	}

	void
	TestTool::Setup(std::vector<std::string> a_fileNames, std::string a_compileArgs)
	{
		m_sources = std::move(a_fileNames);
		
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

		m_diagnosticConsumer = std::make_unique<TestDiagnosticConsumer>();
		m_clangTool->setDiagnosticConsumer(m_diagnosticConsumer.get());

		// Disable Error Output from the clang tool
		SetPrintErrorMessage(false);
	}

	void
	TestTool::Setup(std::string a_fileName, std::string a_compileArgs)
	{
		Setup(std::vector { std::move(a_fileName) }, std::move(a_compileArgs));
	}

	TestTool::~TestTool() {}
}

int
main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
