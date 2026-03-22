#pragma once

#include <memory>
#include <string>
#include <unordered_map>

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
	class ReflectionGenerator;

	class SpyllTool
	{
	protected:
		SpyllTool();

	public:
		using ReflectionGeneratorMap = std::unordered_map<std::string, std::unique_ptr<ReflectionGenerator>>;

		virtual
		~SpyllTool();

		int
		Run();

		int
		GetErrorCode() const { return m_errorCode; }

		void
		SetPrintErrorMessage(bool a_shouldPrintMessage);

		ReflectionGenerator*
		CreateNewReflectionGenerator(std::string_view a_path);

		const ReflectionGeneratorMap&
		GetReflectionGeneratorMap() const;

	protected:
		ReflectionGeneratorMap m_generators;

		std::unique_ptr<clang::tooling::ClangTool> m_clangTool;
		std::unique_ptr<clang::DiagnosticConsumer> m_diagnosticConsumer;

		std::unique_ptr<clang::tooling::FrontendActionFactory> m_action;

		int m_errorCode = -1;
	};
}
