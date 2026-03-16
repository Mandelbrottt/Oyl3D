#pragma once

#include <memory>
#include <string>
#include <vector>

namespace clang::tooling {
	class CompilationDatabase;
}

namespace clang::tooling
{
	class ClangTool;
}

namespace Spyll
{
	class Tool final
	{
	public:
		explicit
		Tool(std::vector<std::string> a_fileNames, std::string a_compileArgs);

		explicit
		Tool(std::string a_fileName, std::string a_compileArgs);

		explicit
		Tool(size_t argc, const char** argv);

		~Tool();

		int
		Run();

		int
		GetErrorCode() const { return m_errorCode; }

	private:
		std::vector<std::string> m_sources;
		std::unique_ptr<clang::tooling::CompilationDatabase> m_compilations;

		std::unique_ptr<clang::tooling::ClangTool> m_clangTool;

		int m_errorCode = -1;
	};
}
