#include "VariableDescriptor.h"

#include "DescriptorSerialization.h"

namespace Spyll
{
	using json = nlohmann::json;

	void
	to_json(json& a_json, const VariableDescriptor& a_descriptor)
	{
		a_json = json {
			OBJ_MEMBER_AS_JSON(a_descriptor, name),
			OBJ_MEMBER_AS_JSON(a_descriptor, id),
			OBJ_MEMBER_AS_JSON(a_descriptor, type),
			OBJ_MEMBER_AS_JSON(a_descriptor, ownerType),
			OBJ_MEMBER_AS_JSON(a_descriptor, ownerFunction),
			OBJ_MEMBER_AS_JSON(a_descriptor, accessSpecifier),
			OBJ_MEMBER_AS_JSON(a_descriptor, isConst),
			OBJ_MEMBER_AS_JSON(a_descriptor, isVolatile),
			OBJ_MEMBER_AS_JSON(a_descriptor, isReference),
			OBJ_MEMBER_AS_JSON(a_descriptor, isPointer),
		};
	}

	void
	from_json(const json& a_json, VariableDescriptor& a_descriptor)
	{
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, name);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, id);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, type);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, ownerType);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, ownerFunction);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, accessSpecifier);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, isConst);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, isVolatile);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, isReference);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, isPointer);
	}
}
