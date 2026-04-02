#pragma warning(disable : 4267)

#include <chrono>

#include "CmdTool.h"

#include "Tool/Core/FileOperations.h"
#include "Tool/Core/ReflectionDescriptor.h"

int
main(int argc, const char** argv)
{
	auto clock = std::chrono::high_resolution_clock::now();

	Spyll::CmdTool tool(argc, argv);
	int result = tool.Run();

	auto run = std::chrono::high_resolution_clock::now();

	auto mergedReflectionDescriptor = tool.GetMergedReflectionDescriptor();

	auto merge = std::chrono::high_resolution_clock::now();

	auto outputFile = tool.GetOutputFile();
	auto binary = tool.ShouldOutputBinary();

	WriteReflectionDescriptorToFile(mergedReflectionDescriptor, outputFile, binary);

	auto write = std::chrono::high_resolution_clock::now();

	printf("Run: %lli milliseconds\n", std::chrono::duration_cast<std::chrono::milliseconds>(run - clock).count());
	printf("Merge: %lli microseconds\n", std::chrono::duration_cast<std::chrono::microseconds>(merge - run).count());
	printf("Write: %lli microseconds\n", std::chrono::duration_cast<std::chrono::microseconds>(write - merge).count());

	return result;
}
