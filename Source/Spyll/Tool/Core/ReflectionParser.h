#pragma once

#include "Declarations/Type.h"
#include "Declarations/Enum.h"
#include "Declarations/Function.h"
#include "Declarations/Variable.h"

namespace clang
{
	class SourceManager;
	class StaticAssertDecl;
}

namespace Spyll
{
	class ReflectionParser final
	{
	public:
		explicit
		ReflectionParser();

		~ReflectionParser();

		bool
		ShouldReflectDecl(const clang::NamedDecl* Decl) const;

		bool
		ParseCXXRecordDecl(clang::CXXRecordDecl* Decl);

		bool
		ParseVarDecl(clang::VarDecl* Decl);

		bool
		ParseFunctionDecl(clang::FunctionDecl* Decl);

		bool
		ParseEnumDecl(clang::EnumDecl* Decl);

		const std::vector<Type*>&
		GetTypes() const
		{
			return m_types;
		}

		const std::vector<Variable*>&
		GetGlobalVariables() const
		{
			return m_globalVariables;
		}

		const std::vector<Function*>&
		GetGlobalFunctions() const
		{
			return m_globalFunctions;
		}

		const std::vector<Enum*>&
		GetEnums() const
		{
			return m_enums;
		}

	private:
		std::vector<Type*> m_types;
		std::vector<Variable*> m_globalVariables;
		std::vector<Function*> m_globalFunctions;
		std::vector<Enum*> m_enums;
	};
}
