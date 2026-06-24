#include "SpyllTool.h"

#include "ReflectionParseAction.h"
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
			ToolFrontendActionFactory(ReflectionParseOptions* a_options)
				: m_options(a_options) {}

			std::unique_ptr<clang::FrontendAction>
			create() override
			{
				return std::make_unique<ReflectionParseAction>(m_options);
			}

		private:
			ReflectionParseOptions* m_options;
		};

		m_action = std::make_unique<ToolFrontendActionFactory>(&m_parseOptions);
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
	SpyllTool::SetReflectionParseOptions(const ReflectionParseOptions& a_options)
	{
		m_parseOptions = a_options;
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
