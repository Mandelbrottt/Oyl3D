#pragma once

#include <memory>

#include <clang/Basic/DiagnosticOptions.h>

#include "ReflectionParseAction.h"
#include "ReflectionParser.h"

#include "Spyll/Tool/Core/ReflectionDescriptor.h"

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
	class SpyllTool
	{
	protected:
		SpyllTool();

	public:
		using OnSetDiagnosticOptionsFn = void(*)(clang::DiagnosticOptions*);

		virtual
		~SpyllTool();

		int
		Run();

		void
		SetReflectionParseOptions(const ReflectionParseOptions& a_options);

		int
		GetErrorCode() const { return m_errorCode; }

		void
		SetPrintErrorMessage(bool a_shouldPrintMessage);

		virtual
		void
		SetDiagnosticOptionsCallback(OnSetDiagnosticOptionsFn a_callback);

		void
		OnSetDiagnosticOptions(clang::DiagnosticOptions* a_diagnosticOptions);

	protected:
		ReflectionParseOptions m_parseOptions;

		OnSetDiagnosticOptionsFn m_diagnosticOptionsFn = nullptr;

		std::unique_ptr<clang::tooling::ClangTool> m_clangTool;
		std::unique_ptr<clang::DiagnosticConsumer> m_diagnosticConsumer;

		std::unique_ptr<clang::tooling::FrontendActionFactory> m_action;

		int m_errorCode = -1;
	};
}
