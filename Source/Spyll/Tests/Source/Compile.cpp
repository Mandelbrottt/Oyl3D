#include <Spyll/Tool.h>

#include <gtest/gtest.h>

#include <array>

TEST(Compile, CompilesHelloWorld)
{
	Spyll::Tool tool("main.cpp", "-x c++");
	int returnCode = tool.Run();

	EXPECT_EQ(returnCode, 0);
}
