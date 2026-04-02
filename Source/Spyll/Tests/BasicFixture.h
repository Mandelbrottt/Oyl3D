#pragma once

#include "Tool/Clang/SpyllTool.h"

class BasicFixture : public testing::Test
{
protected:
	BasicFixture() {}

	Spyll::TestTool tool;
};
