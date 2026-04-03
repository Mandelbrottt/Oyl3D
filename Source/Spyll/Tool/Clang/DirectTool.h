#pragma once

#include "SpyllTool.h"

namespace Spyll
{
	class DirectTool : public SpyllTool
	{
	public:
		DirectTool();

		virtual
		~DirectTool();

		DirectTool(std::vector<std::string> a_fileNames, std::string a_compileArgs);

		DirectTool(std::string a_fileName, std::string a_compileArgs);

		virtual
		void
		Setup(std::vector<std::string> a_fileNames, std::string a_compileArgs);

		virtual
		void
		Setup(std::string a_fileName, std::string a_compileArgs);

	private:
		std::vector<std::string> m_sources;
		std::unique_ptr<clang::tooling::CompilationDatabase> m_compilations;

		std::unique_ptr<clang::DiagnosticConsumer> m_diagnosticConsumer;
	};
}
