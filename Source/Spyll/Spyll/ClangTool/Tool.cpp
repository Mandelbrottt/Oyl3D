#include "pch.h"
#include "Tool.h"

#include "Action.h"

namespace tooling = clang::tooling;
namespace cl = llvm::cl;

namespace Spyll
{
	Tool::~Tool() {}

	int
	Tool::Run()
	{
		if (m_clangTool)
		{
			auto action = tooling::newFrontendActionFactory<DeclFindingAction>();
			m_errorCode = m_clangTool->run(action.get());
		}
		return m_errorCode;
	}

	void
	Tool::SetPrintErrorMessage(bool a_shouldPrintMessage)
	{
		if (m_clangTool)
		{
			m_clangTool->setPrintErrorMessage(a_shouldPrintMessage);
			if (a_shouldPrintMessage)
				m_clangTool->setDiagnosticConsumer(nullptr);
			else
				m_clangTool->setDiagnosticConsumer(m_diagnosticConsumer.get());
		}
	}
}
