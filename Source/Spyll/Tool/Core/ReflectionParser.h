#pragma once

#include <unordered_map>

#include "Declarations/Enum.h"
#include "Declarations/Function.h"
#include "Declarations/Type.h"
#include "Declarations/Variable.h"

namespace clang
{
	class SourceManager;
	class StaticAssertDecl;
	class RecordDecl;
}

namespace Spyll
{
	class ReflectionParser final
	{
	public:
		ReflectionParser() = default;

		bool
		ShouldReflectDecl(const clang::NamedDecl* Decl) const;

		bool
		ParseCXXRecordDecl(clang::CXXRecordDecl* Decl);

		bool
		ParseGlobalVarDecl(clang::VarDecl* Decl);

		bool
		ParseGlobalFunctionDecl(clang::FunctionDecl* Decl);

		bool
		ParseEnumDecl(clang::EnumDecl* Decl);

		Type*
		TryGetParentTypeOfDecl(const clang::NamedDecl* Decl);

		void
		PopulateTypeFields();

		const std::vector<Type>&
		GetTypes() const
		{
			return m_types;
		}

		const std::vector<Variable>&
		GetGlobalVariables() const
		{
			return m_globalVariables;
		}

		const std::vector<Function>&
		GetGlobalFunctions() const
		{
			return m_globalFunctions;
		}

		const std::vector<Enum>&
		GetEnums() const
		{
			return m_enums;
		}

	private:
		std::vector<Type> m_types;
		std::unordered_map<const clang::RecordDecl*, size_t> m_typeIndexMap;

		std::vector<Variable> m_globalVariables;
		std::vector<Function> m_globalFunctions;
		std::vector<Enum> m_enums;
	};
}
