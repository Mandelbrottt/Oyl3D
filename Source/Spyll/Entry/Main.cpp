#include <llvm/Support/CommandLine.h>

#include "CmdTool.h"

#include "Spyll/Spyll/Descriptors/ReflectionDescriptor.h"

int
main(int argc, const char** argv)
{
	Spyll::CmdTool tool(argc, argv);
	int result = tool.Run();

	std::vector<Spyll::ReflectionDescriptor> reflectionDescriptors;
	for (const auto& [fileName, generator] : tool.GetReflectionGeneratorMap())
	{
		reflectionDescriptors.emplace_back(generator->GetReflectionDescriptor());
	}

	auto mergedReflectionDescriptor = CreateMergedReflectionDescriptor(reflectionDescriptors);

	return result;
}
