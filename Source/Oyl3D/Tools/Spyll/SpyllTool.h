#pragma once

#include <string_view>
#include <vector>

#include "Spyll/Tool/Core/OpaqueTool.h"

namespace Oyl
{
	struct SpyllTool : public Spyll::OpaqueTool
	{
		SpyllTool();

		virtual
		~SpyllTool();

		void
		AddInclude(std::string_view a_includeDir);

		void
		AddExternalInclude(std::string_view a_includeDir);

		void
		AddHeaderDeclaration(std::string_view a_sourcePath);

		void
		AddDefine(std::string_view a_define);

		void
		AddDependency(std::string_view a_dependency);

		int
		Run() override;

		std::string_view assemblyName;

		std::vector<std::string_view> includeDirectories;
		std::vector<std::string_view> externalIncludeDirectories;
		std::vector<std::string_view> headerDeclarations;
		std::vector<std::string_view> defines;
		std::vector<std::string_view> dependencies;

		std::string_view pch;
		std::string_view std;
	};
}
