#include "DeclPrinter.h"

#include <algorithm>

#include "StringHelper.h"

std::string
GetTypeNameAsVar(std::string_view a_name)
{
	std::string typeVar = std::string(a_name);
	std::ranges::replace(typeVar, ' ', '_');
	std::ranges::replace(typeVar, '-', '_');
	std::ranges::replace(typeVar, '.', '_');
	std::ranges::replace(typeVar, '<', '_');
	std::ranges::replace(typeVar, '>', '_');
	std::ranges::replace(typeVar, ',', '_');
	FindAndReplace(typeVar, "::", "_");
	FindAndReplace(typeVar, "*", "Ptr");
	FindAndReplace(typeVar, "&", "Ref");

	return typeVar;
}

std::string
GetStolenMemberTypeName(std::string_view a_name)
{
	return "__Stolen__" + GetTypeNameAsVar(a_name);
}

std::string
GetStolenMemberTypeName(const Spyll::Declaration& a_decl)
{
	return GetStolenMemberTypeName(a_decl.GetQualifiedName());
}

std::string
GetStolenMemberTypeName(const Spyll::Function& a_function)
{
	std::string name = GetStolenMemberTypeName(static_cast<const Spyll::Declaration&>(a_function));
	for (const auto& argument : a_function.GetArguments())
	{
		name += "__" + GetTypeNameAsVar(argument.GetTypeAsString());
	}
	return name;
}

std::string
GetStolenMemberTypeName(const Spyll::Method& a_method)
{
	std::string name = GetStolenMemberTypeName(static_cast<const Spyll::Function&>(a_method));
	if (a_method.IsConst())
		name += "__Const";
	return name;
}

std::string
GetSmuggledTypeAsTypeName(std::string_view a_name)
{
	return "__Smuggled__" + GetTypeNameAsVar(a_name);
}
