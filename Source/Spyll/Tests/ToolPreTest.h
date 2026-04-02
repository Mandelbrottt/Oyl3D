#pragma once

#include "Tool/Clang/SpyllTool.h"

template<typename T>
class ToolPreTest : public testing::Test
{
protected:
	const Spyll::TestTool* tool;

	ToolPreTest(std::string a_target, std::string a_args = "-xc++")
		: ToolPreTest(std::vector { std::move(a_target) }, std::move(a_args)) {}

	ToolPreTest(std::vector<std::string> a_targets, std::string a_args = "-xc++")
	{
		tool = TryCreateTool(std::move(a_targets), std::move(a_args));
	}

private:
	Spyll::TestTool*
	TryCreateTool(std::vector<std::string>&& a_targets, std::string&& a_args)
	{
		if (s_tool) 
			return s_tool;

		s_tool = new Spyll::TestTool(a_targets, a_args);
		s_tool->Run();
		return s_tool;
	}

	static Spyll::TestTool* s_tool;
};

template<typename T>
Spyll::TestTool* ToolPreTest<T>::s_tool;