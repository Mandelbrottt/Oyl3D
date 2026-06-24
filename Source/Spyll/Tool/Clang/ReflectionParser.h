#pragma once

#include <clang/AST/ASTConsumer.h>
#include <clang/AST/RecursiveASTVisitor.h>

#include "Declarations/Class.h"
#include "Declarations/Enum.h"
#include "Declarations/Function.h"
#include "Declarations/Variable.h"

namespace Spyll
{
	class ReflectionParser;

	class ReflectionParserConsumer final : public clang::ASTConsumer
	{
	public:
		explicit
		ReflectionParserConsumer(clang::SourceManager& SM, ReflectionParser* Parser);

		void
		HandleTranslationUnit(clang::ASTContext& Ctx) override;

	private:
		ReflectionParser* Parser;
	};

	class ReflectionParser final : public clang::RecursiveASTVisitor<ReflectionParser>
	{
	public:
		explicit
		ReflectionParser();

		~ReflectionParser();

		bool
		ShouldReflectDecl(const clang::NamedDecl* Decl) const;

		bool
		VisitCXXRecordDecl(clang::CXXRecordDecl* Decl);

		bool
		VisitFunctionDecl(clang::FunctionDecl* Decl);

		bool
		VisitVarDecl(clang::VarDecl* Decl);

		bool
		VisitEnumDecl(clang::EnumDecl* Decl);

		clang::SourceManager*
		GetSourceManager() const;

		void
		SetSourceManager(clang::SourceManager* SM);

		clang::ASTContext*
		GetContext() const;

		void
		SetContext(clang::ASTContext* Ctx);

		clang::DiagnosticOptions*
		GetDiagnosticOptions() const;

		void
		SetDiagnosticOptions(clang::DiagnosticOptions* DO);

	private:
		std::string
		GetDeclLocation(clang::SourceLocation Loc) const;

		void
		PrintDecl(const clang::NamedDecl* NamedDecl) const;

	private:
		std::vector<Class*> m_classes;
		std::vector<Function*> m_functions;
		std::vector<Variable*> m_globals;
		std::vector<Enum*> m_enums;

		clang::SourceManager* SourceManager = nullptr;
		clang::ASTContext* Context = nullptr;
		clang::DiagnosticOptions* DiagnosticOptions = nullptr;
	};
}
