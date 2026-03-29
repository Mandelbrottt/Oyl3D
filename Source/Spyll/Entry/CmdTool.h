#pragma once

#include <memory>

#include <clang/Tooling/CommonOptionsParser.h>

#include "Spyll/ClangTool/SpyllTool.h"

namespace Spyll
{
	class CmdTool final : public SpyllTool
	{
	public:
		explicit
		CmdTool(int argc, const char** argv, bool a_printErrors = true);

	private:
		std::unique_ptr<clang::tooling::CommonOptionsParser> m_optionsParser;
	};
}
