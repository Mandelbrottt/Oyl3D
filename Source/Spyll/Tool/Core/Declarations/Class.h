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
	public:
		explicit
		Class(clang::CXXRecordDecl* a_decl, clang::SourceManager* a_sourceManager);

		virtual
		~Class();

	public:
		bool
		ShouldReflect() const override;

		size_t
		GetSize() const;

		size_t
		GetAlignment() const;

		const std::vector<BaseClass>&
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

		std::vector<BaseClass> m_baseClasses;

		//constructors;

		//destructor;

		std::vector<Field> m_fields;

		std::vector<Method> m_methods;

		std::vector<Variable> m_variables;

		std::vector<Function> m_functions;
	};
}
