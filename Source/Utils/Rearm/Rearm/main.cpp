#include <cstdio>
#include <cstring>
#include <vector>

#include "ExportMetadata.h"
#include "IncludeMetadata.h"

enum ReturnCodes
{
	Success = 0,
	NotEnoughArguments,
	InvalidMode,
};

const char* g_exportMode = "export";
const char* g_includeMode = "include";

const char* g_validModes[] = {
	g_exportMode,
	g_includeMode
};

const char* g_currentMode = nullptr;

const char* g_version = "0.0.1";

void
PrintHelp();

int
main(int a_argc, const char* a_argv[])
{
	if (a_argc < 3)
	{
		PrintHelp();
		return NotEnoughArguments;
	}

	g_currentMode = a_argv[1];

	bool valid = false;
	for (int i = 0; i < std::size(g_validModes); i++)
	{
		valid |= _strcmpi(g_currentMode, g_validModes[i]) == 0;
	}

	if (!valid)
	{
		PrintHelp();
		return InvalidMode;
	}

	std::vector<std::string> projectDirs;
	for (int i = 2; i < a_argc; i++)
	{
		projectDirs.push_back(a_argv[i]);
	}

	for (const auto& dir : projectDirs)
	{
		if (_strcmpi(g_currentMode, g_exportMode) == 0)
		{
			GenerateExportReflectionMetaData(dir);
		} else if (_strcmpi(g_currentMode, g_includeMode) == 0)
		{
			GenerateIncludeReflectionMetaData(dir);
		}
	}

	return Success;
}

void
PrintHelp()
{
	printf("Rearm %s\n", g_version);
	printf("\n");
	printf("Usage: Oyl.Rearm <mode> [project root...]\n");
	printf("\n");
	printf("Mode:\n");
	printf("\texport\t\tGenerate reflection export information - For libraries and user projects\n");
	printf("\tinclude\t\tGenerate reflection include information - For use by the entry point\n");
}