#include "DeclPrinter.h"

#include <algorithm>

#include <clang/AST/Decl.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/QualTypeNames.h>

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
	FindAndReplace(typeVar, "(", "LParen_");
	FindAndReplace(typeVar, ")", "_RParen");

	return typeVar;
}

std::string
GetStolenMemberTypeName(std::string_view a_name)
{
	return "__Stolen__" + GetTypeNameAsVar(a_name);
}

std::string
GetStolenMemberTypeName(const clang::NamedDecl& a_decl)
{
	return GetStolenMemberTypeName(a_decl.getQualifiedNameAsString());
}

std::string
GetStolenMemberTypeName(const clang::FunctionDecl& a_function)
{
	auto& ctx = a_function.getASTContext();
	auto printingPolicy = ctx.getPrintingPolicy();

	std::string name = GetStolenMemberTypeName(static_cast<const clang::NamedDecl&>(a_function));
	for (const auto* parameter : a_function.parameters())
	{
		std::string qualifiedName = clang::TypeName::getFullyQualifiedName(parameter->getType(), ctx, printingPolicy);
		name += "__" + GetTypeNameAsVar(qualifiedName);
	}
	return name;
}

std::string
GetStolenMemberTypeName(const clang::CXXMethodDecl& a_method)
{
	std::string name = GetStolenMemberTypeName(static_cast<const clang::FunctionDecl&>(a_method));
	if (a_method.isConst())
		name += "__Const";
	return name;
}

std::string
GetSmuggledTypeAsTypeName(std::string_view a_name)
{
	return "__Smuggled__" + GetTypeNameAsVar(a_name);
}
