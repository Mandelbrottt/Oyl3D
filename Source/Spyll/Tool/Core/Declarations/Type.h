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
	class Type;

	struct BaseType
	{
		std::string name;
	};

	class Type final : public Declaration
	{
	public:
		explicit
		Type(clang::CXXRecordDecl* a_decl);

		virtual
		~Type();

	public:
		bool
		ShouldReflect() const override;

		size_t
		GetSize() const;

		size_t
		GetAlignment() const;

		const std::vector<BaseType>&
		GetBaseClasses() const;

		const std::vector<Field>&
		GetFields() const;

		const std::vector<Method>&
		GetMethods() const;

		const std::vector<Variable>&
		GetVariables() const;

		const std::vector<Function>&
		GetFunctions() const;

	private:
		size_t m_size;
		size_t m_alignment;

		std::vector<BaseType> m_baseClasses;

		//constructors;

		//destructor;

		std::vector<Field> m_fields;

		std::vector<Method> m_methods;

		std::vector<Variable> m_variables;

		std::vector<Function> m_functions;
	};
}
