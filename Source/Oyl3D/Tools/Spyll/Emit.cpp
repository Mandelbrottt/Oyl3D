#include "Emit.h"

#include <filesystem>
#include <fstream>

#include "SpyllTool.h"

void
EmitCodeFromTool(const Oyl::SpyllTool& a_tool)
{
	auto descriptor = a_tool.GetMergedReflectionDescriptor();

	std::filesystem::create_directory("Generated");

	std::ofstream generatedIncludeHeader;
	generatedIncludeHeader.open("Generated/Spyll.generated.h");
	generatedIncludeHeader << "#pragma once\n";

	auto emitPragmaMessage = [&](std::string_view a_message)
	{
		generatedIncludeHeader << "#pragma message(\"";
		generatedIncludeHeader << a_message;
		generatedIncludeHeader << "\")\n";
	};

	emitPragmaMessage(a_tool.std);
	emitPragmaMessage(a_tool.pch);
}
