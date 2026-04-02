#pragma once

#include <memory>

#include <llvm/Support/CommandLine.h>

#include "Tool/Clang/SpyllTool.h"

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
extern llvm::cl::OptionCategory g_spyllToolCategory;

namespace clang::tooling
{
	class CommonOptionsParser;
}

namespace Spyll
{
	class CmdTool final : public SpyllTool
	{
	public:
		explicit
		CmdTool(int argc, const char** argv, bool a_printErrors = true);

		virtual
		~CmdTool();

		std::string_view
		GetOutputFile() const;

		bool
		ShouldOutputBinary() const;

	private:
		llvm::Error
		Init(int argc, const char** argv);

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
