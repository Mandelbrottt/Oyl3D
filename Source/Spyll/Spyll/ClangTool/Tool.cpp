#include "pch.h"
#include "Tool.h"

#include "Action.h"

namespace tooling = clang::tooling;
namespace cl = llvm::cl;

namespace Spyll
{
	Tool::Tool()
	{
		class ToolFrontendActionFactory : public tooling::FrontendActionFactory
		{
		public:
			explicit
			ToolFrontendActionFactory(ReflectionGenerator* Generator)
				: Generator(Generator) {}

			std::unique_ptr<clang::FrontendAction>
			create() override
			{
				return std::make_unique<DeclFindingAction>(Generator);
			}

		private:
			ReflectionGenerator* Generator;
		};

		m_reflectionGenerator = std::make_unique<ReflectionGenerator>();
		m_action = std::make_unique<ToolFrontendActionFactory>(m_reflectionGenerator.get());

		// Disable Error Output from the clang tool
		m_diagnosticConsumer = std::make_unique<clang::IgnoringDiagConsumer>();
	}

	Tool::~Tool() {}

	int
	Tool::Run()
	{
		if (m_clangTool)
		{
			m_errorCode = m_clangTool->run(m_action.get());
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
