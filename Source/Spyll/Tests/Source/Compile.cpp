#include "pch.h"

#include <Spyll/Tool.h>

TEST(Compile, CompilesHelloWorld)
{
	{
		Spyll::DirectTool tool("Main.cpp", "-x c++");
		int returnCode = tool.Run();

		EXPECT_EQ(returnCode, 0);
	}
	{
		Spyll::DirectTool tool("Fake.File", "-x c++");
		int returnCode = tool.Run();

		EXPECT_EQ(returnCode, 1);
	}
}
