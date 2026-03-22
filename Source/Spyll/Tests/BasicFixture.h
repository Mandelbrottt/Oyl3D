#pragma once

#include "Spyll/ClangTool/SpyllTool.h"

class BasicFixture : public testing::Test
{
protected:
	BasicFixture() {}

	Spyll::TestTool tool;
};
