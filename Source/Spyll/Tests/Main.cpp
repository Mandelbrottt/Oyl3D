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

	TestTool::TestTool() {}

	TestTool::~TestTool() {}

	TestTool::TestTool(std::vector<std::string> a_fileNames, std::string a_compileArgs)
		: DirectTool(std::move(a_fileNames), std::move(a_compileArgs)) {}

	TestTool::TestTool(std::string a_fileName, std::string a_compileArgs)
		: DirectTool(std::move(a_fileName), std::move(a_compileArgs)) {}

	void
	TestTool::Setup(std::vector<std::string> a_fileNames, std::string a_compileArgs)
	{
		DirectTool::Setup(std::move(a_fileNames), std::move(a_compileArgs));

		m_diagnosticConsumer = std::make_unique<TestDiagnosticConsumer>();
		m_clangTool->setDiagnosticConsumer(m_diagnosticConsumer.get());

		// Disable Error Output from the clang tool
		SetPrintErrorMessage(false);
	}

	void
	TestTool::Setup(std::string a_fileName, std::string a_compileArgs)
	{
		DirectTool::Setup(std::move(a_fileName), std::move(a_compileArgs));
	}

}

int
main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
