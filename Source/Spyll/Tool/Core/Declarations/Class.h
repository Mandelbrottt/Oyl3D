#pragma once
#pragma once

#include "Function.h"
#include "Method.h"
#include "Record.h"
#include "Variable.h"

namespace clang
{
	class CXXRecordDecl;
}

namespace Spyll
{
	class Class;

	struct BaseDescriptor
	{
		Record* type;
		bool isVirtual;
	};

	class Class : public Record
	{
		friend class ReflectionParser;

	public:
		explicit
		Class(clang::CXXRecordDecl* a_decl);

		bool
		ShouldReflect() const override;

		const std::vector<BaseDescriptor>&
		GetBases() const { return m_bases; }

		const std::vector<Variable>&
		GetVariables() const { return m_variables; }

		const std::vector<Method>&
		GetMethods() const { return m_methods; }

		const std::vector<Function>&
		GetFunctions() const { return m_functions; }

		const clang::CXXRecordDecl*
		GetClangDecl() const;

	private:
		// Set by ReflectionParser
		std::vector<BaseDescriptor> m_bases;

		//constructors;

		//destructor;

		std::vector<Method> m_methods;

		std::vector<Variable> m_variables;

		std::vector<Function> m_functions;
	};
}
