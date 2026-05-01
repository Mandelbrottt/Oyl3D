#pragma once

#include <string_view>

enum ReturnCodes
{
	SUCCESS                = 0,
	INVALID_NAMED_ARGUMENT = 1 << 4
};

using ArgFn = void(*)(std::string_view);

bool
RegisterPositionalArgumentHandler(ArgFn a_fn);

bool
RegisterNamedArgumentHandler(std::string_view a_arg, ArgFn a_fn);

int
ProcessArgs(int argc, char** argv);
