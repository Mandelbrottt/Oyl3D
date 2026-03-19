#pragma once

#include <memory>
#include <string>
#include <vector>

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

	class CmdTool final : public Tool
	{
	public:
		explicit
		CmdTool(int argc, const char** argv, bool a_printErrors = true);

		virtual
		~CmdTool();

	private:
		std::unique_ptr<clang::tooling::CommonOptionsParser> m_optionsParser;
	};

	class DirectTool final : public Tool
	{
	public:
		explicit
		DirectTool(std::vector<std::string> a_fileNames, std::string a_compileArgs);

		explicit
		DirectTool(std::string a_fileName, std::string a_compileArgs);

		virtual
		~DirectTool();

	private:
		std::vector<std::string> m_sources;
		std::unique_ptr<clang::tooling::CompilationDatabase> m_compilations;
	};
}
