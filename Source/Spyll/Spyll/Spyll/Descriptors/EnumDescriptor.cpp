#include "EnumDescriptor.h"

#include "DescriptorSerialization.h"

namespace Spyll
{
	using json = nlohmann::json;

	void
	to_json(json& a_json, const EnumDescriptor& a_descriptor)
	{
		a_json = json {
			OBJ_MEMBER_AS_JSON(a_descriptor, name),
			OBJ_MEMBER_AS_JSON(a_descriptor, id),
			OBJ_MEMBER_AS_JSON(a_descriptor, underlyingType),
			OBJ_MEMBER_AS_JSON(a_descriptor, enumerations),
		};
	}

	void
	from_json(const json& a_json, EnumDescriptor& a_descriptor)
	{
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, name);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, id);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, underlyingType);
		OBJ_MEMBER_FROM_JSON(a_json, a_descriptor, enumerations);
	}
}
