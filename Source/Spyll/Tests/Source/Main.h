#pragma once

#include "Spyll/Tool.h"

namespace Spyll
{
	class TestTool final : public Tool
	{
	public:
		explicit
		TestTool(std::vector<std::string> a_fileNames, std::string a_compileArgs);

		explicit
		TestTool(std::string a_fileName, std::string a_compileArgs);

		virtual
		~TestTool();

	private:
		std::vector<std::string> m_sources;
		std::unique_ptr<clang::tooling::CompilationDatabase> m_compilations;
	};
}
