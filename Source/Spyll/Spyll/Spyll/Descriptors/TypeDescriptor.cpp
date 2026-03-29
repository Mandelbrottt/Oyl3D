#include "TypeDescriptor.h"

#include "DescriptorSerialization.h"

namespace Spyll
{
	using json = nlohmann::json;

	static
	void
	to_json(json& a_json, const BaseTypeDescriptor& a_descriptor)
	{
		a_json = json {
			OBJ_MEMBER_AS_JSON(a_descriptor, type),
			OBJ_MEMBER_AS_JSON(a_descriptor, accessSpecifier),
			OBJ_MEMBER_AS_JSON(a_descriptor, isVirtual),
		};
	}

	static
	void
	from_json(const json& a_json, BaseTypeDescriptor& a_descriptor)
	{
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, type);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, accessSpecifier);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, isVirtual);
	}

	void
	to_json(json& a_json, const TypeDescriptor& a_descriptor)
	{
		a_json = json {
			OBJ_MEMBER_AS_JSON(a_descriptor, name),
			OBJ_MEMBER_AS_JSON(a_descriptor, id),
			OBJ_MEMBER_AS_JSON(a_descriptor, alignment),
			OBJ_MEMBER_AS_JSON(a_descriptor, isComposite),
			OBJ_MEMBER_AS_JSON(a_descriptor, isStruct),
			OBJ_MEMBER_AS_JSON(a_descriptor, isOpaque),
			OBJ_MEMBER_AS_JSON(a_descriptor, baseTypes),
		};
	}

	void
	from_json(const json& a_json, TypeDescriptor& a_descriptor)
	{
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, name);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, id);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, alignment);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, isComposite);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, isStruct);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, isOpaque);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, baseTypes);
	}
}
