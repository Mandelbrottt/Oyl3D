#include "FileOperations.h"

#include "ReflectionDescriptor.h"

#include "DescriptorSerialization.h"

#include <fstream>
#include <iomanip>

namespace Spyll
{
	using json = nlohmann::json;

	void
	WriteReflectionDescriptorToFile(
		const ReflectionDescriptor& a_descriptor,
		std::string_view a_fileName,
		bool a_binary
	)
	{
		json reflectionDescriptorJson = json();
		to_json(reflectionDescriptorJson, a_descriptor);

		printf("Outputting descriptor file to %s", a_fileName.data());

		std::ofstream output;
		if (a_binary)
		{
			output.open(a_fileName.data(), std::ios::out | std::ios::binary);
			auto bin = json::to_msgpack(reflectionDescriptorJson);
			output.write(reinterpret_cast<const char*>(bin.data()), bin.size());
		} else
		{
			output.open(a_fileName.data(), std::ios::out);
			output << std::setw(4) << reflectionDescriptorJson;
		}

		output.close();
	}

	ReflectionDescriptor
	ReadReflectionDescriptorFromFile(std::string_view a_fileName, bool a_binary)
	{
		ReflectionDescriptor result;

		auto inputFlags = std::ios::in;
		if (a_binary) inputFlags |= std::ios::binary;

		auto input = std::ifstream(a_fileName.data(), inputFlags);
		json obj;
		if (a_binary)
		{
			obj = json::from_msgpack(input);
		} else
		{
			obj = json::parse(input);
		}

		from_json(obj, result);

		input.close();

		return result;
	}
}
