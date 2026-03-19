#include "pch.h"

#include <Spyll/Clang/Tool.h>

TEST(Compile, CompilesHelloWorld)
{
	{
		Spyll::TestTool tool("Main.cpp", "-x c++");
		int returnCode = tool.Run();

		EXPECT_EQ(returnCode, 0);
	}
	{
		Spyll::TestTool tool("Fake.File", "-x c++");
		int returnCode = tool.Run();

		EXPECT_EQ(returnCode, 1);
	}
}
