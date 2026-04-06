#include <cstdio>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "Spyll/Tool/Clang/OpaqueTool.h"

static std::vector<std::string_view> g_positionalArgs;
static std::unordered_map<std::string, std::string> g_optArgs;

void
ProcessArgs(int argc, char** argv)
{
	for (int i = 0; i < argc; i++)
	{
		std::string_view rawArg = argv[i];
		if (!rawArg.find("-", 0, 1))
		{
			g_positionalArgs.emplace_back(rawArg);
			continue;
		}

		int startIndex = 0;
		while (rawArg[startIndex] == '-')
		{
			++startIndex;
		}

		char argNameBuf[512] {};
		char argValueBuf[512] {};
		int nScanned = sscanf_s(rawArg.data() + startIndex, "%s=%s", argNameBuf, argValueBuf);
		if (nScanned != 2)
		{
			fprintf(stderr, "Invalid argument '%s'", rawArg.data());
			continue;
		}

		g_optArgs[argNameBuf] = argValueBuf;
	}
}

int
main(int argc, char** argv)
{
	ProcessArgs(argc, argv);

	return 0;
}
