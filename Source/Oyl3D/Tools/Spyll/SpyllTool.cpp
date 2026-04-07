#include "SpyllTool.h"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace Oyl
{
	SpyllTool::SpyllTool() {}

	SpyllTool::~SpyllTool() {}

	void
	SpyllTool::AddInclude(std::string_view a_includeDir)
	{
		includeDirectories.emplace_back(std::move(a_includeDir));
	}

	void
	SpyllTool::AddExternalInclude(std::string_view a_includeDir)
	{
		externalIncludeDirectories.emplace_back(std::move(a_includeDir));
	}

	void
	SpyllTool::AddHeaderDeclaration(std::string_view a_sourcePath)
	{
		headerDeclarations.emplace_back(std::move(a_sourcePath));
	}

	void
	SpyllTool::AddDefine(std::string_view a_define)
	{
		defines.emplace_back(std::move(a_define));
	}

	int
	SpyllTool::Run()
	{
		// Ensure file gets deleted in the event of an error
		struct CreateAndDeleteTempFile
		{
			CreateAndDeleteTempFile(std::string_view a_fileName, const std::vector<std::string_view>& a_headers)
			{
				tempFile = a_fileName;

				fileStream.open(a_fileName.data());
				for (auto headerDeclaration : a_headers)
				{
					fileStream << "#include \"" << headerDeclaration << "\"\n";
				}
				fileStream.close();
			}

			~CreateAndDeleteTempFile()
			{
				std::filesystem::remove(tempFile);
			}

			std::ofstream fileStream;
			std::string_view tempFile;
		};

		const CreateAndDeleteTempFile createAndDeleteFile("_temp.gen.cpp", headerDeclarations);

		std::ostringstream argumentsStream;

		argumentsStream << "-xc++\n";

		if (!std.empty())
		{
			argumentsStream << "-std=" << std << "\n";
		}

		argumentsStream << "-Wno-everything\n";
		argumentsStream << "-D__REFLECT_GENERATE__=1\n";
		
		if (!pch.empty())
		{
			argumentsStream << "-include" << pch << "\n";
			if (auto iter = std::find(headerDeclarations.begin(), headerDeclarations.end(), pch); 
				iter != headerDeclarations.end())
			{
				headerDeclarations.erase(iter);
			}
		}

		for (auto includeDir : includeDirectories)
		{
			argumentsStream << "-I" << includeDir << "\n";
		}

		for (auto externalIncludeDir : externalIncludeDirectories)
		{
			argumentsStream << "-isystem" << externalIncludeDir << "\n";
		}

		for (auto define : defines)
		{
			argumentsStream << "-D" << define << "\n";
		}

		auto arguments = argumentsStream.str();

		//printf("Sources: ");
		//for (const auto& string : m_sourceFiles)
		//{
		//	printf("%s, ", string.data());
		//}

		//printf("\nArguments: %s", arguments.data());

		std::string source = std::string { createAndDeleteFile.tempFile };
		Setup(source, arguments);

		return OpaqueTool::Run();
	}
}
