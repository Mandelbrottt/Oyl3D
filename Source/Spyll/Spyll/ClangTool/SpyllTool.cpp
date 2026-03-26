#include "pch.h"
#include "SpyllTool.h"

#include "Action.h"

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
			ToolFrontendActionFactory(SpyllTool* Tool)
				: m_tool(Tool) {}

			std::unique_ptr<clang::FrontendAction>
			create() override
			{
				return std::make_unique<DeclFindingAction>(m_tool);
			}

		private:
			SpyllTool* m_tool;
		};

		m_action = std::make_unique<ToolFrontendActionFactory>(this);

		// Disable Error Output from the clang tool
		//m_diagnosticConsumer = std::make_unique<clang::IgnoringDiagConsumer>();
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

	ReflectionGenerator*
	SpyllTool::CreateNewReflectionGenerator(std::string_view a_path)
	{
		auto [kvp, _] = m_generators.emplace(a_path, std::make_unique<ReflectionGenerator>());
		return kvp->second.get();
	}

	const SpyllTool::ReflectionGeneratorMap&
	SpyllTool::GetReflectionGeneratorMap() const
	{
		return m_generators;
	}

	ReflectionDescriptor
	SpyllTool::GetMergedReflectionDescriptor() const
	{
		std::vector<ReflectionDescriptor> reflectionDescriptors;
		for (const auto& [fileName, generator] : m_generators)
		{
			reflectionDescriptors.emplace_back(generator->GetReflectionDescriptor());
		}
		return CreateMergedReflectionDescriptor(reflectionDescriptors);
	}
}
