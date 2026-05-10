#include "SpyllTool.h"

#include "ReflectionParser.h"

namespace tooling = clang::tooling;
namespace cl = llvm::cl;

namespace Spyll
{
	SpyllTool::SpyllTool()
	{
		class ToolFrontendActionFactory : public tooling::FrontendActionFactory
		{
		public:
			explicit
			ToolFrontendActionFactory(ReflectionParser* Parser)
				: m_parser(Parser) {}

			std::unique_ptr<clang::FrontendAction>
			create() override
			{
				return std::make_unique<ReflectionParserAction>(m_parser);
			}

		private:
			ReflectionParser* m_parser;
		};

		m_action = std::make_unique<ToolFrontendActionFactory>(&m_reflectionParser);
	}

	SpyllTool::~SpyllTool() {}

	int
	SpyllTool::Run()
	{
		if (m_clangTool)
		{
			m_errorCode = m_clangTool->run(m_action.get());
		}
		return m_errorCode;
	}

	void
	SpyllTool::SetPrintErrorMessage(bool a_shouldPrintMessage)
	{
		if (m_clangTool)
		{
			m_clangTool->setPrintErrorMessage(a_shouldPrintMessage);
		}
	}

	void
	SpyllTool::SetDiagnosticOptionsCallback(OnSetDiagnosticOptionsFn a_callback)
	{
		m_diagnosticOptionsFn = a_callback;
	}

	void
	SpyllTool::OnSetDiagnosticOptions(clang::DiagnosticOptions* a_diagnosticOptions)
	{
		if (m_diagnosticOptionsFn)
		{
			m_diagnosticOptionsFn(a_diagnosticOptions);
		}
	}
}
