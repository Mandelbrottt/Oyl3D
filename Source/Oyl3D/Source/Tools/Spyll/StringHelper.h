#pragma once

#include <sstream>
#include <string>

std::string&
FindAndReplace(std::string& a_str, std::string_view a_find, std::string_view a_replace);

void
AddIndentToStringStream(std::stringstream& a_stream, std::string_view a_indent = "\t");

constexpr
std::string_view
GetTrimmedStringView(std::string_view a_view)
{
	size_t start = a_view.find_first_not_of(" \n\t");
	size_t end = a_view.find_last_not_of(" \n\t");
	return a_view.substr(start, end - start + 1);
}

void
PushIndent(std::string& a_indent);

void
PopIndent(std::string& a_indent);

