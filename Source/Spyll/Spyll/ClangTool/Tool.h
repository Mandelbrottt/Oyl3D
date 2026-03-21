#pragma once

#include <memory>

#include "Spyll/ClangTool/ReflectionGenerator.h"

namespace clang
{
	class DiagnosticConsumer;

	namespace tooling
	{
		class ClangTool;
		class CommonOptionsParser;
		class CompilationDatabase;
		class FrontendActionFactory;
	}
}

namespace Spyll
{
	class Tool
	{
	protected:
		Tool();

	public:
		virtual
		~Tool();

		int
		Run();

		int
		GetErrorCode() const { return m_errorCode; }

		void
		SetPrintErrorMessage(bool a_shouldPrintMessage);

		const ReflectionGenerator*
		GetReflectionGenerator() const
		{
			return m_reflectionGenerator.get();
		}

	protected:
		std::unique_ptr<ReflectionGenerator> m_reflectionGenerator;

		std::unique_ptr<clang::tooling::ClangTool> m_clangTool;
		std::unique_ptr<clang::DiagnosticConsumer> m_diagnosticConsumer;

		std::unique_ptr<clang::tooling::FrontendActionFactory> m_action;

		int m_errorCode = -1;
	};
}
