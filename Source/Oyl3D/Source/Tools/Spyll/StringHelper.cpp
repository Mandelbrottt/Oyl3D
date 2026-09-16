#include "StringHelper.h"

std::string&
FindAndReplace(std::string& a_str, std::string_view a_find, std::string_view a_replace)
{
	size_t start_pos = 0;
	while ((start_pos = a_str.find(a_find, start_pos)) != std::string::npos)
	{
		a_str.replace(start_pos, a_find.length(), a_replace);
		start_pos += a_replace.length(); // Handles case where 'replace' is a substring of 'find'
	}
	return a_str;
};

void
AddIndentToStringStream(std::stringstream& a_stream, std::string_view a_indent)
{
	a_stream.seekg(0, std::ios::end);
	auto size = a_stream.tellg();
	a_stream.seekg(0);

	if (size == 0)
	{
		return;
	}

	std::stringstream indentStream;
	while (!a_stream.eof())
	{
		char buf[1024] {};
		a_stream.getline(buf, std::size(buf) - 1);
		indentStream << a_indent << buf;
		if (a_stream.peek(), !a_stream.eof())
		{
			indentStream << "\n";
		}
	}
	a_stream.swap(indentStream);
	a_stream.seekp(0, std::ios::end);
}

void
PushIndent(std::string& a_indent)
{
	a_indent += "\t";
}

void
PopIndent(std::string& a_indent)
{
	a_indent = a_indent.substr(0, a_indent.size() - 1);
}
