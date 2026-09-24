#pragma once

#include <sstream>
#include <string>

#include <clang/Basic/Specifiers.h>

#include <Spyll/Tool/Core/Declarations/Declaration.h>
#include <Spyll/Tool/Core/Declarations/Function.h>
#include <Spyll/Tool/Core/Declarations/Method.h>

std::string
GetTypeNameAsVar(std::string_view a_name);

std::string
GetStolenMemberTypeName(std::string_view a_name);

std::string
GetStolenMemberTypeName(const clang::NamedDecl& a_decl);

std::string
GetStolenMemberTypeName(const clang::FunctionDecl& a_function);

std::string
GetStolenMemberTypeName(const clang::CXXMethodDecl& a_method);

template<typename T>
std::string
StolenMember(const T& a_value)
{
	std::stringstream call;
	call << "(Oyl::Reflection::Internal::StolenMember_V<" << GetStolenMemberTypeName(a_value) << ">)";
	return call.str();
}

std::string
GetSmuggledTypeAsTypeName(std::string_view a_name);

namespace std
{
	inline
	std::string
	to_string(clang::AccessSpecifier a_accessSpecifier)
	{
		std::string_view strings[(size_t) Spyll::AccessSpecifier::Count];
		strings[clang::AS_public] = "Public";
		strings[clang::AS_protected] = "Protected";
		strings[clang::AS_private] = "Private";
		strings[clang::AS_none] = "Global";
		return std::string(strings[(size_t) a_accessSpecifier]);
	}
}