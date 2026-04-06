#include "OpaqueTool.h"

#include <clang/Tooling/CompilationDatabase.h>

#include "DirectTool.h"

namespace Spyll
{
	struct OpaqueTool::Impl
	{
		std::unique_ptr<DirectTool> tool;

		std::vector<std::string> sources;
		std::unique_ptr<clang::tooling::CompilationDatabase> compilations;

		std::unique_ptr<clang::DiagnosticConsumer> diagnosticConsumer;
	};

	OpaqueTool::OpaqueTool()
	{
		m_impl = new Impl;
		m_impl->tool = std::make_unique<DirectTool>();
	}

	OpaqueTool::~OpaqueTool()
	{
		delete m_impl;
		m_impl = nullptr;
	}

	OpaqueTool::OpaqueTool(std::vector<std::string> a_fileNames, std::string a_compileOptions)
		: OpaqueTool()
	{
		Setup(std::move(a_fileNames), std::move(a_compileOptions));
	}

	OpaqueTool::OpaqueTool(std::string a_fileName, std::string a_compileOptions)
		: OpaqueTool()
	{
		Setup(std::move(a_fileName), std::move(a_compileOptions));
	}

	void
	OpaqueTool::Setup(std::vector<std::string> a_fileNames, std::string a_compileOptions)
	{
		m_impl->tool->Setup(std::move(a_fileNames), std::move(a_compileOptions));
	}

	void
	OpaqueTool::Setup(std::string a_fileName, std::string a_compileOptions)
	{
		Setup(std::vector { std::move(a_fileName) }, std::move(a_compileOptions));
	}

	int
	OpaqueTool::Run()
	{
		m_impl->tool->SetDiagnosticOptionsCallback(
			[](auto options)
			{
				options->VerifyDiagnostics = false;
				options->IgnoreWarnings = true;
				options->ShowCarets = false;
			}
		);
		m_impl->tool->SetPrintErrorMessage(false);
		return m_impl->tool->Run();
	}

	int
	OpaqueTool::GetErrorCode() const
	{
		return m_impl->tool->GetErrorCode();
	}

	void
	OpaqueTool::SetPrintErrorMessage(bool a_shouldPrintMessage)
	{
		m_impl->tool->SetPrintErrorMessage(a_shouldPrintMessage);
	}

	ReflectionDescriptor
	OpaqueTool::GetMergedReflectionDescriptor() const
	{
		return m_impl->tool->GetMergedReflectionDescriptor();
	}
}
