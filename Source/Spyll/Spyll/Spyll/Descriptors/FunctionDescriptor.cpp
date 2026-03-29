#include "FunctionDescriptor.h"

#include "DescriptorSerialization.h"

namespace Spyll
{
	using json = nlohmann::json;

	void
	to_json(json& a_json, const FunctionDescriptor& a_descriptor)
	{
		a_json = json {
			OBJ_MEMBER_AS_JSON(a_descriptor, name),
			OBJ_MEMBER_AS_JSON(a_descriptor, id),
			OBJ_MEMBER_AS_JSON(a_descriptor, returnType),
			OBJ_MEMBER_AS_JSON(a_descriptor, ownerType),
			OBJ_MEMBER_AS_JSON(a_descriptor, accessSpecifier),
			OBJ_MEMBER_AS_JSON(a_descriptor, constructorType),
			OBJ_MEMBER_AS_JSON(a_descriptor, isVirtual),
			OBJ_MEMBER_AS_JSON(a_descriptor, isPureVirtual),
			OBJ_MEMBER_AS_JSON(a_descriptor, isOverride),
			OBJ_MEMBER_AS_JSON(a_descriptor, isStatic),
			OBJ_MEMBER_AS_JSON(a_descriptor, isConst),
			OBJ_MEMBER_AS_JSON(a_descriptor, isVolatile),
			OBJ_MEMBER_AS_JSON(a_descriptor, isReturnConst),
			OBJ_MEMBER_AS_JSON(a_descriptor, isReturnVolatile),
			OBJ_MEMBER_AS_JSON(a_descriptor, isReturnReference),
			OBJ_MEMBER_AS_JSON(a_descriptor, isReturnPointer),
		};
	}

	void
	from_json(const json& a_json, FunctionDescriptor& a_descriptor)
	{
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, name);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, id);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, returnType);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, ownerType);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, accessSpecifier);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, constructorType);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, isVirtual);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, isPureVirtual);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, isOverride);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, isStatic);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, isConst);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, isVolatile);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, isReturnConst);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, isReturnVolatile);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, isReturnReference);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, isReturnPointer);
	}
}
