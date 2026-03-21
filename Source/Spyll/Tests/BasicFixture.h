#pragma once

#include <Spyll/ClangTool/Tool.h>

class BasicFixture : public testing::Test
{
protected:
	BasicFixture() {}

	Spyll::TestTool tool;
};
