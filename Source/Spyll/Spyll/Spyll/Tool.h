#pragma once

#include <memory>

namespace clang
{
	class DiagnosticConsumer;

	namespace tooling
	{
		class ClangTool;
		class CommonOptionsParser;
		class CompilationDatabase;
	}
}

namespace Spyll
{
	class Tool
	{
	public:
		virtual
		~Tool();

		int
		Run();

		int
		GetErrorCode() const { return m_errorCode; }

		void
		SetPrintErrorMessage(bool a_shouldPrintMessage);

	protected:
		Tool() = default;

		std::unique_ptr<clang::tooling::ClangTool> m_clangTool;
		std::unique_ptr<clang::DiagnosticConsumer> m_diagnosticConsumer;

		int m_errorCode = -1;
	};
}
