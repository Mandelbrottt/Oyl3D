#include "Cmd.h"

#include <string>
#include <vector>
#include <unordered_map>

static std::string g_callingProgram;
static std::vector<std::string_view> g_positionalArgs;
static std::vector<std::pair<std::string_view, std::string_view>> g_optArgs;

static ArgFn g_positionalArgHandler;
static std::unordered_map<std::string_view, ArgFn> g_namedArgHandlers;

int
ProcessPositionalArg(std::string_view a_arg);

int
ProcessNamedArg(std::string_view a_arg);

int
ProcessArgs(int argc, char** argv)
{
	g_callingProgram = argv[0];

	// Ignore calling program arg after caching
	argc--;
	argv++;

	int returnCode = SUCCESS;

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
ProcessPositionalArg(std::string_view a_arg)
{
	g_positionalArgs.emplace_back(a_arg);

	if (g_positionalArgHandler)
		g_positionalArgHandler(a_arg);

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

	if (auto iter = g_namedArgHandlers.find(argName); iter != g_namedArgHandlers.end())
	{
		auto namedArgHandler = iter->second;
		namedArgHandler(argValue);
	}

	return SUCCESS;
}

bool
RegisterPositionalArgumentHandler(ArgFn a_fn)
{
	g_positionalArgHandler = a_fn;
	return true;
}

bool
RegisterNamedArgumentHandler(std::string_view a_arg, ArgFn a_fn)
{
	auto [iter, inserted] = g_namedArgHandlers.emplace(a_arg, a_fn);
	return inserted;
}
