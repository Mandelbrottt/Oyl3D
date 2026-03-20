#pragma once

#include <string>
#include <vector>

#include "Spyll/Spyll/TypeId.h"

namespace Spyll::Reflection
{
	class Assembly;
	class Function;
	class Field;
	
	class Type final
	{
	public:
		~Type();

	private:
		Type();

		TypeId m_typeId;

		uint32_t m_size = 0;
		
		std::string m_name;
		std::string m_qualifiedName;

		Assembly* m_assembly;

		std::vector<Function*> m_functions;
		std::vector<Field*> m_fields;
	};
}
