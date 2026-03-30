#include "Field.h"

#include "Type.h"

namespace Spyll::Reflection
{
	struct Field::Impl
	{
		std::string name;
		std::string qualifiedName;

		Type* type = nullptr;

		Type* ownerType = nullptr;
		uint32_t offset;

		AccessSpecifier accessSpecifier = AccessSpecifier::None;

		bool isConst = false;
		bool isVolatile = false;
		bool isReference = false;
		bool isPointer = false;
	};
}
