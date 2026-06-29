#pragma once

#include "Tool/Core/SpyllTool.h"

class BasicFixture : public testing::Test
{
protected:
	BasicFixture() {}

	Spyll::TestTool tool;
};
