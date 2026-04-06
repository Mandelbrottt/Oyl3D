#include <string>
#include <unordered_map>
#include <vector>

#include "Spyll/Tool/Clang/OpaqueTool.h"

static std::string g_callingProgram;
static std::vector<std::string_view> g_positionalArgs;
static std::unordered_map<std::string, std::string> g_optArgs;

enum ReturnCodes
{
	SUCCESS = 0,
	INVALID_NAMED_ARGUMENT = 1 << 4
};

int
ProcessPositionalArg(std::string_view a_arg)
{
	g_positionalArgs.emplace_back(a_arg);
	return SUCCESS;
}

int
ProcessNamedArg(std::string_view a_arg)
{
	size_t startIndex = 0;
	while (a_arg[startIndex] == '-')
	{
		++startIndex;
	}

	size_t eqIndex = a_arg.find("=", startIndex);

	std::string_view argName = a_arg.substr(startIndex, eqIndex - startIndex);

	std::string_view argValue;
	if (eqIndex != std::string::npos)
	{
		argValue = a_arg.substr(eqIndex + 1);
	}

	g_optArgs.emplace(argName, argValue);

	return SUCCESS;
}

int
ProcessArgs(int argc, char** argv)
{
	int returnCode = 0;

	for (int i = 0; i < argc; i++)
	{
		std::string_view rawArg = argv[i];
		if (rawArg.find("-", 0, 1) == std::string::npos)
		{
			returnCode |= ProcessPositionalArg(rawArg);
			continue;
		}

		returnCode |= ProcessNamedArg(rawArg);
	}

	return returnCode;
}

int
main(int argc, char** argv)
{
	g_callingProgram = argv[0];
	auto returnCode = ProcessArgs(argc - 1, argv + 1);
	if (returnCode != 0)
	{
		return returnCode;
	}

	return 0;
}
