#include "BasicFixture.h"

class RecordsTest : public testing::Test
{
protected:
	static Spyll::TestTool* tool;

	RecordsTest()
	{
		if (tool) return;

		tool = new Spyll::TestTool("SimpleInlineRecords.cpp", "-x c++");
		tool->Run();
	}
};

Spyll::TestTool* RecordsTest::tool = nullptr;

TEST_F(RecordsTest, TestRun)
{
	EXPECT_EQ(0, 0);
}
