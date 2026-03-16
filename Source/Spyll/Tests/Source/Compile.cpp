#include "pch.h"

#include <Spyll/Tool.h>

TEST(Compile, CompilesHelloWorld)
{
	Spyll::Tool tool("main.cpp", "-x c++");
	int returnCode = tool.Run();

	EXPECT_EQ(returnCode, 0);
}
