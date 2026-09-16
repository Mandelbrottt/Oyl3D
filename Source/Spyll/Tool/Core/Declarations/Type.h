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

	struct BaseDescriptor
	{
		Type* type;
		bool isVirtual;
	};

	class Type final : public Declaration
	{
		friend class ReflectionParser;

	public:
		explicit
		Type(clang::CXXRecordDecl* a_decl, Type* a_parent = nullptr);

	public:
		bool
		ShouldReflect() const override;

		size_t
		GetSize() const { return m_size; }

		size_t
		GetAlignment() const { return m_alignment; }

		const std::vector<BaseDescriptor>&
		GetBaseTypes() const { return m_baseTypes; }

		const std::vector<Field>&
		GetFields() const { return m_fields; }

		const std::vector<Method>&
		GetMethods() const { return m_methods; }

		const std::vector<Variable>&
		GetVariables() const { return m_variables; }

		const std::vector<Function>&
		GetFunctions() const { return m_functions; }

		const clang::CXXRecordDecl*
		GetClangDecl() const;

	private:
		size_t m_size;
		size_t m_alignment;

		// Set by ReflectionParser
		std::vector<BaseDescriptor> m_baseTypes;

		//constructors;

		//destructor;

		std::vector<Field> m_fields;

		std::vector<Method> m_methods;

		std::vector<Variable> m_variables;

		std::vector<Function> m_functions;
	};
}
