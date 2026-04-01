#pragma warning(disable : 4267)

#include <fstream>
#include <iomanip>
#include <iostream>

#include <llvm/Support/CommandLine.h>

#include <nlohmann/json.hpp>

#include "CmdTool.h"

#include "Spyll/Spyll/DescriptorSerialization.h"
#include "Spyll/Spyll/ReflectionDescriptor.h"

int
main(int argc, const char** argv)
{
	Spyll::CmdTool tool(argc, argv);
	int result = tool.Run();

	auto mergedReflectionDescriptor = tool.GetMergedReflectionDescriptor();
	nlohmann::json reflectionDescriptorJson = nlohmann::json();
	to_json(reflectionDescriptorJson, mergedReflectionDescriptor);

	printf("Outputting descriptor file to %s", tool.GetOutputFile().data());

	std::ofstream output;
	if (tool.ShouldOutputBinary())
	{
		output.open(tool.GetOutputFile().data(), std::ios::out | std::ios::binary);
		auto bin = nlohmann::json::to_msgpack(reflectionDescriptorJson);
		output.write(reinterpret_cast<const char*>(bin.data()), bin.size());
	} else
	{
		output.open(tool.GetOutputFile().data(), std::ios::out);
		output << std::setw(4) << reflectionDescriptorJson;
	}

	output.close();

	return result;
}
