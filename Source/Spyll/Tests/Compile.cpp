#include "BasicFixture.h"

namespace
{
	struct CompileTest : BasicFixture
	{
		void SetUp() override
		{
			tool.SetDiagnosticOptionsCallback(
				[](clang::DiagnosticOptions* a_options)
				{
					a_options->ShowCarets = false;
					a_options->UseANSIEscapeCodes = true;
				}
			);
		}
	};
}

TEST_F(CompileTest, CompilesHelloWorld)
{
	tool.Setup("HelloWorld.cpp", "-x c++");
	int returnCode = tool.Run();

	EXPECT_EQ(returnCode, 0);
}

TEST_F(CompileTest, FailOnSyntaxError)
{
	tool.Setup("SyntaxError.cpp", "-x c++");
	int returnCode = tool.Run();

	EXPECT_EQ(returnCode, 1);
}

TEST_F(CompileTest, FailOnFakeFile)
{
	tool.Setup("Fake.file", "-x c++");
	int returnCode = tool.Run();

	EXPECT_EQ(returnCode, 1);
}
