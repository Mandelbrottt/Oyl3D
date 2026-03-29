#include <llvm/Support/CommandLine.h>

#pragma warning(push)
#pragma warning(disable : 4267)
#include <nlohmann/json.hpp>
#pragma warning(pop)

#include "CmdTool.h"

#include "Spyll/Spyll/Descriptors/ReflectionDescriptor.h"

#include "Spyll/Spyll/Descriptors/DescriptorSerialization.h"

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

	auto mergedReflectionDescriptor = tool.GetMergedReflectionDescriptor();

	nlohmann::json reflectionDescriptorJson = mergedReflectionDescriptor;

	auto bin = nlohmann::json::to_msgpack(reflectionDescriptorJson);

	nlohmann::json descriptorFromJson = nlohmann::json::from_msgpack(bin);

	Spyll::ReflectionDescriptor fromDescriptor = descriptorFromJson.get<Spyll::ReflectionDescriptor>();

	return result;
}
