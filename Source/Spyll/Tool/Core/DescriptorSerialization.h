#pragma once

#include <nlohmann/json_fwd.hpp>

#include "EnumDescriptor.h"
#include "FieldDescriptor.h"
#include "FunctionDescriptor.h"
#include "ReflectionDescriptor.h"
#include "TypeDescriptor.h"
#include "VariableDescriptor.h"

#define OBJ_MEMBER_AS_JSON(_json_, _obj_, _name_) _json_[#_name_] = _obj_._name_
#define OBJ_MEMBER_FROM_JSON(_json_, _obj_, _name_) _json_.at(#_name_).get_to(_obj_._name_)

#define OBJ_OPTIONAL_MEMBER_AS_JSON(_json_, _obj_, _name_) \
	if (!!_obj_._name_) \
		_json_[#_name_] = _obj_._name_

#define OBJ_OPTIONAL_MEMBER_FROM_JSON(_json_, _obj_, _name_) \
	if (auto iter = _json_.find(#_name_); iter != _json_.end())\
		iter->get_to(_obj_._name_)

#define JSON_ENUM_HELPER_FNS(_name_) \
	template<typename BasicJsonType> \
	void \
	to_json(BasicJsonType& a_json, const _name_& a_id) \
	{ \
		a_json = static_cast<std::underlying_type_t<_name_>>(a_id); \
	} \
	\
	template<typename BasicJsonType> \
	void \
	from_json(const BasicJsonType& a_json, _name_& a_id) \
	{ \
		a_id = static_cast<_name_>(a_json.template get<std::underlying_type_t<_name_>>()); \
	} \

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

	JSON_ENUM_HELPER_FNS(TypeDescriptorId)

	JSON_ENUM_HELPER_FNS(FieldDescriptorId)

	JSON_ENUM_HELPER_FNS(FunctionDescriptorId)

	JSON_ENUM_HELPER_FNS(VariableDescriptorId)

	JSON_ENUM_HELPER_FNS(EnumDescriptorId)

	JSON_ENUM_HELPER_FNS(Reflection::AccessSpecifier)

	JSON_ENUM_HELPER_FNS(Reflection::ConstructorType)
}