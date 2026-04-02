#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include <clang/Basic/DiagnosticOptions.h>

#include "ReflectionGenerator.h"

#include "Tool/Core/ReflectionDescriptor.h"

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
		using ReflectionGeneratorMap = std::unordered_map<std::string, std::unique_ptr<ReflectionGenerator>>;
		using OnSetDiagnosticOptionsFn = void(*)(clang::DiagnosticOptions*);

		virtual
		~SpyllTool();

		int
		Run();

		int
		GetErrorCode() const { return m_errorCode; }

		void
		SetPrintErrorMessage(bool a_shouldPrintMessage);

		virtual
		void
		SetDiagnosticOptionsCallback(OnSetDiagnosticOptionsFn a_callback);

		void
		OnSetDiagnosticOptions(clang::DiagnosticOptions* a_diagnosticOptions);

		ReflectionGenerator*
		CreateNewReflectionGenerator(std::string_view a_path);

		const ReflectionGeneratorMap&
		GetReflectionGeneratorMap() const;

		ReflectionDescriptor
		GetMergedReflectionDescriptor() const;

	protected:
		ReflectionGeneratorMap m_generators;

		OnSetDiagnosticOptionsFn m_diagnosticOptionsFn = nullptr;

		std::unique_ptr<clang::tooling::ClangTool> m_clangTool;
		std::unique_ptr<clang::DiagnosticConsumer> m_diagnosticConsumer;

		std::unique_ptr<clang::tooling::FrontendActionFactory> m_action;

		int m_errorCode = -1;
	};
}
