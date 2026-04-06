#pragma once

#include <vector>

#include "Spyll/Tool/Clang/OpaqueTool.h"

namespace Oyl
{
	class SpyllTool : protected Spyll::OpaqueTool
	{
	public:
		SpyllTool();

		virtual
		~SpyllTool();

		void
		AddInclude(std::string_view a_includeDir);

		void
		AddExternalInclude(std::string_view a_includeDir);

		void
		AddSource(std::string_view a_sourcePath);

		void
		UsePrecompiledHeader(std::string_view a_value);

		int
		Run();

	private:
		std::vector<std::string_view> m_includeDirectories;
		std::vector<std::string_view> m_externalIncludeDirectories;
		std::vector<std::string_view> m_sourceFiles;

		std::string_view m_pch;
	};
}