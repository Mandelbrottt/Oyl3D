#include "ExportMetadata.h"

#include <filesystem>

namespace std
{
	namespace fs = filesystem;
}

static
void
ProcessFile(const std::fs::path& a_projectDir, const std::fs::path& a_path);


void
GenerateExportReflectionMetaData(const std::fs::path& a_projectDir)
{
}

void
ProcessFile(const std::fs::path& a_projectDir, const std::fs::path& a_path)
{
	
}}
std::string
GetFileText(const std::fs::path& a_path)
{
	constexpr auto read_size = size_t(4096);

	std::ifstream stream(a_path);
	stream.exceptions(std::ios_base::badbit);

	if (!stream)
	{
		fprintf(stderr, "Could not open file\"%ls\"!", std::fs::absolute(a_path).c_str());
		return std::string();
	}

	// Oyl currently only supports ansi source files
	std::string text;
	std::string buf(read_size, '\0');
	while (stream.read(&buf[0], read_size))
	{
		text.append(buf, 0, stream.gcount());
	}
	text.append(buf, 0, stream.gcount());
	return text;
}
