#pragma once

#include "Declaration.h"
#include "Field.h"
#include "Function.h"
#include "Method.h"
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

	class Class final : public Declaration
	{
		friend class Field;
		friend class Method;

		friend class Variable;
		friend class Function;

	public:
		explicit
		Class(clang::CXXRecordDecl* a_decl);

		virtual
		~Class();

		bool
		ShouldReflect() const override;

	private:
		size_t m_size;
		size_t m_alignment;

		std::vector<BaseClass> m_baseClasses;

		//constructors;

		//destructor;

		std::vector<Field> m_fields;

		std::vector<Method> m_methods;

		std::vector<Variable> m_variables;

		std::vector<Function> m_functions;
	};
}
