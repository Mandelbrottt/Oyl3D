#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "SpyllTool.h"

#include "Spyll/Tool/Clang/OpaqueTool.h"

static std::string g_callingProgram;
static std::vector<std::string_view> g_positionalArgs;
static std::vector<std::pair<std::string_view, std::string_view>> g_optArgs;

enum ReturnCodes
{
	SUCCESS                = 0,
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

	g_optArgs.emplace_back(argName, argValue);

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

template<typename TPred>
void
ForEachStringInDelimitedList(std::string_view a_listString, char a_separator, TPred&& a_predicate)
{
	size_t index = 0;
	while (index < a_listString.size())
	{
		size_t nextIndex = a_listString.find_first_of(a_separator, index + 1);
		size_t count = std::string::npos;
		if (nextIndex != std::string::npos)
		{
			count = nextIndex - index;
		}

		std::string_view currentString = a_listString.substr(index, count);
		if (!currentString.empty())
		{
			a_predicate(currentString);
		}

		if (nextIndex == std::string::npos)
		{
			break;
		}

		index = nextIndex + 1;

		while (index < a_listString.size() && a_listString[index] == a_separator)
		{
			index++;
		}
	}
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

	Oyl::SpyllTool tool;

	for (const auto& [arg, value] : g_optArgs)
	{
		if (arg == "include")
		{
			ForEachStringInDelimitedList(
				value,
				';',
				[&](std::string_view a_string) { tool.AddInclude(a_string); }
			);
		}

		if (arg == "externalinclude")
		{
			ForEachStringInDelimitedList(
				value,
				';',
				[&](std::string_view a_string) { tool.AddExternalInclude(a_string); }
			);
		}

		if (arg == "pch" && !value.empty())
		{
			tool.pch = value;
		}

		if (arg == "std" && !value.empty())
		{
			tool.std = value;
		}
	}

	for (const auto& source : g_positionalArgs)
	{
		ForEachStringInDelimitedList(
			source,
			';',
			[&](std::string_view a_string) { tool.AddHeaderDeclaration(a_string); }
		);
	}

	returnCode = tool.Run();
	if (returnCode != SUCCESS)
	{
		return returnCode;
	}

	auto descriptor = tool.GetMergedReflectionDescriptor();

	std::filesystem::create_directory("Generated");

	std::ofstream generatedIncludeHeader;
	generatedIncludeHeader.open("Generated/Spyll.generated.h");
	generatedIncludeHeader << "#pragma once\n";
	generatedIncludeHeader << R"(#pragma message("Hello from Spyll.generated.h!"))" << "\n";

	return 0;
}
