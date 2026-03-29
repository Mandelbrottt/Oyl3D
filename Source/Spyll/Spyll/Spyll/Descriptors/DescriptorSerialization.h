#pragma once

#include <nlohmann/json_fwd.hpp>

#include "EnumDescriptor.h"
#include "FieldDescriptor.h"
#include "FunctionDescriptor.h"
#include "ReflectionDescriptor.h"
#include "TypeDescriptor.h"
#include "VariableDescriptor.h"

#define OBJ_MEMBER_AS_JSON(_obj_, _name_) { #_name_, _obj_._name_ }
#define OBJ_MEMBER_FROM_JSON(_json_, _obj_, _name_) _json_.at(#_name_).get_to(_obj_._name_)

namespace Spyll
{
	// ReflectionDescriptor
	void
	to_json(nlohmann::json& a_json, const ReflectionDescriptor& a_descriptor);

	void
	from_json(const nlohmann::json& a_json, ReflectionDescriptor& a_descriptor);
	// ReflectionDescriptor

	// TypeDescriptor
	void
	to_json(nlohmann::json& a_json, const TypeDescriptor& a_descriptor);

	void
	from_json(const nlohmann::json& a_json, TypeDescriptor& a_descriptor);
	// TypeDescriptor

	// FieldDescriptor
	void
	to_json(nlohmann::json& a_json, const FieldDescriptor& a_descriptor);

	void
	from_json(const nlohmann::json& a_json, FieldDescriptor& a_descriptor);
	// FieldDescriptor

	// FunctionDescriptor
	void
	to_json(nlohmann::json& a_json, const FunctionDescriptor& a_descriptor);

	void
	from_json(const nlohmann::json& a_json, FunctionDescriptor& a_descriptor);
	// FunctionDescriptor

	// VariableDescriptor
	void
	to_json(nlohmann::json& a_json, const VariableDescriptor& a_descriptor);

	void
	from_json(const nlohmann::json& a_json, VariableDescriptor& a_descriptor);
	// VariableDescriptor

	// EnumDescriptor
	void
	to_json(nlohmann::json& a_json, const EnumDescriptor& a_descriptor);

	void
	from_json(const nlohmann::json& a_json, EnumDescriptor& a_descriptor);
	// EnumDescriptor
}