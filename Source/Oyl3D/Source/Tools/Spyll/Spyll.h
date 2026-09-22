#pragma once

#pragma warning(disable : 4702) // Unreachable code in MSVC
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/Tooling.h>
#pragma warning(pop)

namespace Spyll
{
	struct ReflectionContext
	{
		std::vector<clang::CXXRecordDecl*> classes;
		std::vector<clang::VarDecl*> globalVariables;
		std::vector<clang::FunctionDecl*> globalFunctions;
		std::vector<clang::EnumDecl*> enums;

		clang::IntrusiveRefCntPtr<clang::SourceManager> sourceManager;
		clang::IntrusiveRefCntPtr<clang::ASTContext> astContext;
	};

	class ReflectionVisitor final : public clang::RecursiveASTVisitor<ReflectionVisitor>
	{
	public:
		explicit
		ReflectionVisitor(clang::CompilerInstance& CI, clang::ASTContext& Ctx, ReflectionContext& RC)
			: m_sourceManager(CI.getSourceManager()),
			  m_context(Ctx),
			  m_diagnosticOptions(CI.getDiagnosticOpts()),
			  m_reflectionContext(RC) {}

		bool
		ShouldReflectDecl(clang::NamedDecl* Decl) const;

		bool
		VisitCXXRecordDecl(clang::CXXRecordDecl* Decl);

		bool
		VisitFunctionDecl(clang::FunctionDecl* Decl);

		bool
		VisitVarDecl(clang::VarDecl* Decl);

		bool
		VisitEnumDecl(clang::EnumDecl* Decl);

		clang::SourceManager&
		GetSourceManager() const { return m_sourceManager; }

		clang::ASTContext&
		GetContext() const { return m_context; }

		clang::DiagnosticOptions&
		GetDiagnosticOptions() const { return m_diagnosticOptions; }

	private:
		std::string
		GetDeclLocation(clang::SourceLocation Loc) const;

		void
		PrintDecl(clang::NamedDecl* NamedDecl) const;

	private:
		clang::SourceManager& m_sourceManager;
		clang::ASTContext& m_context;
		clang::DiagnosticOptions& m_diagnosticOptions;

		ReflectionContext& m_reflectionContext;
	};

	class ReflectionConsumer final : public clang::ASTConsumer
	{
	public:
		explicit
		ReflectionConsumer(clang::CompilerInstance& CI, ReflectionContext& a_reflectionContext)
			: m_compilerInstance(CI),
			  m_reflectionContext(a_reflectionContext) {}

		void
		HandleTranslationUnit(clang::ASTContext& Ctx) override
		{
			auto printingPolicy = Ctx.getPrintingPolicy();
			printingPolicy.Bool = true;
			printingPolicy.FullyQualifiedName = true;
			printingPolicy.SuppressScope = false;
			printingPolicy.SuppressUnwrittenScope = false;
			Ctx.setPrintingPolicy(printingPolicy);

			ReflectionVisitor Visitor(m_compilerInstance, Ctx, m_reflectionContext);
			Visitor.TraverseTranslationUnitDecl(Ctx.getTranslationUnitDecl());
		}

	private:
		clang::CompilerInstance& m_compilerInstance;
		ReflectionContext& m_reflectionContext;
	};

	class ReflectionAction final : public clang::ASTFrontendAction
	{
	public:
		ReflectionAction(ReflectionContext& a_reflectionContext)
			: m_reflectionContext(a_reflectionContext) {}

		std::unique_ptr<clang::ASTConsumer>
		CreateASTConsumer(
			clang::CompilerInstance& CI,
			llvm::StringRef InFile
		) override
		{
			m_reflectionContext.sourceManager = CI.getSourceManagerPtr();
			m_reflectionContext.astContext = CI.getASTContextPtr();

			auto& opts = CI.getDiagnosticOpts();
			opts.VerifyDiagnostics = false;
			opts.IgnoreWarnings = true;
			opts.ShowCarets = false;
			return std::make_unique<ReflectionConsumer>(CI, m_reflectionContext);
		}

	private:
		ReflectionContext& m_reflectionContext;
	};

	class ReflectionActionFactory : public clang::tooling::FrontendActionFactory
	{
	public:
		explicit
		ReflectionActionFactory(ReflectionContext& a_reflectionContext)
			: m_reflectionContext(a_reflectionContext) {}

		std::unique_ptr<clang::FrontendAction>
		create() override
		{
			return std::make_unique<ReflectionAction>(m_reflectionContext);
		}

	private:
		ReflectionContext& m_reflectionContext;
	};
}
