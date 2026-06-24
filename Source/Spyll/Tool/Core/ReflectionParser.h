#pragma once

#include "Declarations/Class.h"
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
		ParseFunctionDecl(clang::FunctionDecl* Decl, clang::SourceManager* a_sourceManager);

		bool
		ParseVarDecl(clang::VarDecl* Decl, clang::SourceManager* a_sourceManager);

		bool
		ParseEnumDecl(clang::EnumDecl* Decl, clang::SourceManager* a_sourceManager);

		const std::vector<Class*>&
		GetClasses() const
		{
			return m_classes;
		}

		const std::vector<Function*>&
		GetFunctions() const
		{
			return m_functions;
		}

		const std::vector<Variable*>&
		GetGlobals() const
		{
			return m_globals;
		}

		const std::vector<Enum*>&
		GetEnums() const
		{
			return m_enums;
		}

	private:
		std::vector<Class*> m_classes;
		std::vector<Function*> m_functions;
		std::vector<Variable*> m_globals;
		std::vector<Enum*> m_enums;
	};
}
