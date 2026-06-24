#pragma once

#include "Declaration.h"
#include "Field.h"
#include "Function.h"
#include "Method.h"
#include "Type.h"
#include "Variable.h"

namespace clang
{
	class CXXRecordDecl;
}

namespace Spyll
{
	class Class;

	struct BaseClass
	{
		std::string name;
	};

	class Class final : public Type
	{
		friend class Field;
		friend class Method;

		friend class Variable;
		friend class Function;

	public:
		explicit
		Class(clang::CXXRecordDecl* a_decl, bool a_isOpaque);

		virtual
		~Class();

		bool
		ShouldReflect() const override;

	private:
		bool m_isOpaque;

		std::vector<BaseClass> m_baseClasses;

		//constructors;

		//destructor;

		std::vector<Field> m_fields;

		std::vector<Method> m_methods;

		std::vector<Variable> m_variables;

		std::vector<Function> m_functions;
	};
}
