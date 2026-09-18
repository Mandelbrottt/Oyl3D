#pragma once

#include <unordered_map>

#include "Declarations/Enum.h"
#include "Declarations/Function.h"
#include "Declarations/Class.h"
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
		ParseRecordDecl(clang::RecordDecl* Decl);

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
		PostProcess();

		const std::vector<Type*>&
		GetTypes() const { return m_types; }

		const std::vector<Type>&
		GetPrimitives() const { return m_primitives; }

		const std::vector<Record>&
		GetRecords() const { return m_records; }

		const std::vector<Class>&
		GetClasses() const { return m_classes; }

		const std::vector<Enum>&
		GetEnums() const { return m_enums; }

		const std::vector<Variable>&
		GetGlobalVariables() const { return m_globalVariables; }

		const std::vector<Function>&
		GetGlobalFunctions() const { return m_globalFunctions; }

	private:
		void
		ParseClassMemberFieldAndFunctionTypes();

		void
		PopulateTypeFields();

	private:
		std::vector<Type*> m_types;

		std::vector<Type> m_primitives;
		std::vector<Record> m_records;
		std::vector<Class> m_classes;
		std::vector<Enum> m_enums;

		std::vector<Variable> m_globalVariables;
		std::vector<Function> m_globalFunctions;

		std::unordered_map<const clang::TypeDecl*, size_t> m_typeIndexMap;
	};
}
