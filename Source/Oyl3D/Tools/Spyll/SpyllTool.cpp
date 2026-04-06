#include "SpyllTool.h"

#include <filesystem>
#include <sstream>

namespace Oyl
{
	SpyllTool::SpyllTool() {}

	SpyllTool::~SpyllTool() {}

	void
	SpyllTool::AddInclude(std::string_view a_includeDir)
	{
		m_includeDirectories.emplace_back(std::move(a_includeDir));
	}

	void
	SpyllTool::AddExternalInclude(std::string_view a_includeDir)
	{
		m_externalIncludeDirectories.emplace_back(std::move(a_includeDir));
	}

	void
	SpyllTool::AddSource(std::string_view a_sourcePath)
	{
		m_sourceFiles.emplace_back(std::move(a_sourcePath));
	}

	void
	SpyllTool::UsePrecompiledHeader(std::string_view a_value)
	{
		m_pch = a_value;
	}

	int
	SpyllTool::Run()
	{
		std::ostringstream argumentsStream;

		argumentsStream << "-xc++\n";
		argumentsStream << "-Wno-everything\n";
		argumentsStream << "-D__REFLECT__=1\n";
		
		if (!m_pch.empty())
		{
			argumentsStream << "-include";
			argumentsStream << m_pch;
			argumentsStream << "\n";

			auto pchSource = std::filesystem::path(m_pch).replace_extension("cpp").string();

			auto _ = std::remove_if(
				m_sourceFiles.begin(),
				m_sourceFiles.end(),
				[&](std::string_view a_value) { return a_value.find(pchSource) != std::string::npos; }
			);
		}

		for (auto includeDirectory : m_includeDirectories)
		{
			argumentsStream << "-I";
			argumentsStream << includeDirectory;

			argumentsStream << "\n";
		}
		for (auto externalIncludeDirectory : m_externalIncludeDirectories)
		{
			argumentsStream << "-isystem";
			argumentsStream << externalIncludeDirectory;

			argumentsStream << "\n";
		}

		auto arguments = argumentsStream.str();

		//printf("Sources: ");
		//for (const auto& string : m_sourceFiles)
		//{
		//	printf("%s, ", string.data());
		//}

		//printf("\nArguments: %s", arguments.data());

		std::vector<std::string> sources = { m_sourceFiles.begin(), m_sourceFiles.end() };
		Setup(sources, arguments);

		return OpaqueTool::Run();
	}
}
