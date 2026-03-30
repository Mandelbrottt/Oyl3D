#pragma once

#include "Field.h"
#include "Function.h"
#include "Type.h"

namespace Spyll::Reflection
{
	class Assembly final
	{
		friend
		void
		::_PopulateAssembly(Assembly* a_assemblyLocation);

		Assembly();

	public:
		~Assembly();

	private:
		std::vector<Type> m_types;

		std::vector<Field> m_fields;

		std::vector<Function> m_functions;
	};
}
