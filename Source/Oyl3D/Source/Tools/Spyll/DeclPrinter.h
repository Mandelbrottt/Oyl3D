#pragma once

#include <sstream>
#include <string>

#include <Spyll/Tool/Core/Declarations/Declaration.h>
#include <Spyll/Tool/Core/Declarations/Function.h>
#include <Spyll/Tool/Core/Declarations/Method.h>

std::string
GetTypeNameAsVar(std::string_view a_name);

std::string
GetStolenMemberTypeName(std::string_view a_name);

std::string
GetStolenMemberTypeName(const Spyll::Declaration& a_decl);

std::string
GetStolenMemberTypeName(const Spyll::Function& a_function);

std::string
GetStolenMemberTypeName(const Spyll::Method& a_method);

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