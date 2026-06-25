#pragma once

#include "Declarations/Type.h"
#include "Declarations/Enum.h"
#include "Declarations/Function.h"
#include "Declarations/Variable.h"

namespace clang
{
	class SourceManager;
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
		ParseCXXRecordDecl(clang::CXXRecordDecl* Decl, clang::SourceManager* a_sourceManager);

		bool
		ParseVarDecl(clang::VarDecl* Decl, clang::SourceManager* a_sourceManager);

		bool
		ParseFunctionDecl(clang::FunctionDecl* Decl, clang::SourceManager* a_sourceManager);

		bool
		ParseEnumDecl(clang::EnumDecl* Decl, clang::SourceManager* a_sourceManager);

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
