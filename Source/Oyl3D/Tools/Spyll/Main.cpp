#include "Cmd.h"
#include "Emit.h"
#include "SpyllTool.h"

static Oyl::SpyllTool g_tool;

template<typename TPred>
void
ForEachStringInDelimitedList(std::string_view a_listString, char a_separator, TPred&& a_predicate);

void
AssemblyHandler(std::string_view a_value)
{
	g_tool.assemblyName = a_value;
}

void
DependencyHandler(std::string_view a_value)
{
	ForEachStringInDelimitedList(
		a_value,
		';',
		[&](std::string_view a_string) { g_tool.AddDependency(a_string); }
	);
}

void
IncludeHandler(std::string_view a_value)
{
	ForEachStringInDelimitedList(
		a_value,
		';',
		[&](std::string_view a_string) { g_tool.AddInclude(a_string); }
	);
}

void
ExternalIncludeHandler(std::string_view a_value)
{
	ForEachStringInDelimitedList(
		a_value,
		';',
		[&](std::string_view a_string) { g_tool.AddExternalInclude(a_string); }
	);
}

void
PchHandler(std::string_view a_value)
{
	g_tool.pch = a_value;
}

void
StdHandler(std::string_view a_value)
{
	g_tool.std = a_value;
}

void
PositionalArgumentHandler(std::string_view a_value)
{
	ForEachStringInDelimitedList(
		a_value,
		';',
		[&](std::string_view a_string) { g_tool.AddHeaderDeclaration(a_string); }
	);
}

int
main(int argc, char** argv)
{
	RegisterPositionalArgumentHandler(PositionalArgumentHandler);

	RegisterNamedArgumentHandler("assembly", AssemblyHandler);
	RegisterNamedArgumentHandler("dependency", DependencyHandler);

	RegisterNamedArgumentHandler("include", IncludeHandler);
	RegisterNamedArgumentHandler("externalinclude", ExternalIncludeHandler);

	RegisterNamedArgumentHandler("pch", PchHandler);
	RegisterNamedArgumentHandler("std", StdHandler);

	auto returnCode = ProcessArgs(argc, argv);
	if (returnCode != SUCCESS)
	{
		return returnCode;
	}

	returnCode = g_tool.Run();
	if (returnCode != SUCCESS)
	{
		return returnCode;
	}

	EmitCodeFromTool(g_tool);

	return 0;
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
