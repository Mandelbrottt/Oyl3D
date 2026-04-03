#include "DirectTool.h"

#include <clang/Tooling/CompilationDatabase.h>

#include <filesystem>

namespace Spyll
{
	DirectTool::DirectTool() {}

	DirectTool::~DirectTool() {}

	DirectTool::DirectTool(std::vector<std::string> a_fileNames, std::string a_compileArgs)
	{
		Setup(std::move(a_fileNames), std::move(a_compileArgs));
	}

	DirectTool::DirectTool(std::string a_fileName, std::string a_compileArgs)
	{
		Setup(std::move(a_fileName), std::move(a_compileArgs));
	}

	void
	DirectTool::Setup(std::vector<std::string> a_fileNames, std::string a_compileArgs)
	{
		m_sources = std::move(a_fileNames);
		
		std::replace(a_compileArgs.begin(), a_compileArgs.end(), ' ', '\n');
		std::string error;
		m_compilations = clang::tooling::FixedCompilationDatabase::loadFromBuffer(
			std::filesystem::current_path().string(),
			a_compileArgs,
			error
		);

		m_clangTool = std::make_unique<clang::tooling::ClangTool>(
			*m_compilations,
			m_sources
		);
	}

	void
	DirectTool::Setup(std::string a_fileName, std::string a_compileArgs)
	{
		Setup(std::vector { std::move(a_fileName) }, std::move(a_compileArgs));
	}

}