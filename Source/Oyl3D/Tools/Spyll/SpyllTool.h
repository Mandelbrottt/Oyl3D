#pragma once

#include <vector>

#include "Spyll/Tool/Clang/OpaqueTool.h"

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

		int
		Run() override;

		std::vector<std::string_view> includeDirectories;
		std::vector<std::string_view> externalIncludeDirectories;
		std::vector<std::string_view> headerDeclarations;
		std::vector<std::string_view> defines;

		std::string_view pch;
		std::string_view std;
	};
}