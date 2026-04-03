#pragma once

#include "Tool/Clang/DirectTool.h"

namespace Spyll
{
	class TestTool final : public DirectTool
	{
	public:
		TestTool();

		virtual
		~TestTool();

		TestTool(std::vector<std::string> a_fileNames, std::string a_compileArgs);

		TestTool(std::string a_fileName, std::string a_compileArgs);

		void
		Setup(std::vector<std::string> a_fileNames, std::string a_compileArgs) override;

		void
		Setup(std::string a_fileName, std::string a_compileArgs) override;

	private:
		std::vector<std::string> m_sources;
		std::unique_ptr<clang::tooling::CompilationDatabase> m_compilations;

		std::unique_ptr<clang::DiagnosticConsumer> m_diagnosticConsumer;
	};
}
