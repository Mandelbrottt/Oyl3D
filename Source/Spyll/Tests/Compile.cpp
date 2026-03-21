#include "BasicFixture.h"

struct CompileTest : BasicFixture {};

TEST_F(CompileTest, CompilesHelloWorld)
{
	tool.Setup("HelloWorld.cpp", "-x c++");
	int returnCode = tool.Run();

	EXPECT_EQ(returnCode, 0);
}

TEST_F(CompileTest, SucceedsOnSyntaxError)
{
	tool.Setup("SyntaxError.cpp", "-x c++");
	int returnCode = tool.Run();

	EXPECT_EQ(returnCode, 0);
}

TEST_F(CompileTest, FailOnFakeFile)
{
	tool.Setup("Fake.file", "-x c++");
	int returnCode = tool.Run();

	EXPECT_EQ(returnCode, 1);
}
