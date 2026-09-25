#pragma once

#pragma warning(push, 1)
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/Tooling.h>
#pragma warning(pop)

namespace Spyll
{
	template<typename TDecl>
		requires (std::is_convertible_v<std::add_pointer_t<TDecl>, std::add_pointer_t<clang::NamedDecl>>)
	struct DeclDescriptor
	{
		explicit
		DeclDescriptor(TDecl* Decl)
			: decl(Decl)
		{
			name = Decl->getNameAsString();
			qualifiedName = Decl->getQualifiedNameAsString();
		}

		TDecl* decl;
		std::string name;
		std::string qualifiedName;
	};

	struct ReflectionContext
	{
		std::string assemblyName;
		std::vector<std::string> dependencies;

		std::vector<DeclDescriptor<clang::CXXRecordDecl>> classes;
		std::vector<DeclDescriptor<clang::VarDecl>> globalVariables;
		std::vector<DeclDescriptor<clang::FunctionDecl>> globalFunctions;
		std::vector<DeclDescriptor<clang::EnumDecl>> enums;

		const clang::SourceManager&
		GetSourceManager() const { return *m_sourceManager; }

		const clang::ASTContext&
		GetASTContext() const { return *m_astContext; }

		const clang::TargetInfo&
		GetTargetInfo() const { return *m_target; }

	private:
		friend class ReflectionAction;

		void
		Init(clang::CompilerInstance& CI)
		{
			m_sourceManager = CI.getSourceManagerPtr();
			m_astContext = CI.getASTContextPtr();
			m_target = CI.getTargetPtr();
			m_fileManager = CI.getFileManagerPtr();
		}

		clang::IntrusiveRefCntPtr<clang::SourceManager> m_sourceManager;
		clang::IntrusiveRefCntPtr<clang::ASTContext> m_astContext;
		clang::IntrusiveRefCntPtr<clang::TargetInfo> m_target;
		clang::IntrusiveRefCntPtr<clang::FileManager> m_fileManager;
	};

	using OnHandleSourceFileFn = void(const ReflectionContext&);

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
		ReflectionConsumer(
			clang::CompilerInstance& CI,
			ReflectionContext& a_reflectionContext,
			OnHandleSourceFileFn* a_callback
		)
			: m_compilerInstance(CI),
			  m_reflectionContext(a_reflectionContext),
			  m_callback(a_callback) {}

		void
		HandleTranslationUnit(clang::ASTContext& Ctx) override
		{
			auto printingPolicy = Ctx.getPrintingPolicy();
			printingPolicy.Bool = true;
			printingPolicy.FullyQualifiedName = true;
			printingPolicy.PrintCanonicalTypes = true;
			printingPolicy.SuppressScope = false;
			printingPolicy.SuppressUnwrittenScope = false;
			Ctx.setPrintingPolicy(printingPolicy);

			ReflectionVisitor Visitor(m_compilerInstance, Ctx, m_reflectionContext);
			Visitor.TraverseTranslationUnitDecl(Ctx.getTranslationUnitDecl());

			if (m_callback)
				m_callback(m_reflectionContext);
		}

	private:
		clang::CompilerInstance& m_compilerInstance;
		ReflectionContext& m_reflectionContext;
		OnHandleSourceFileFn* m_callback;
	};

	class ReflectionAction final : public clang::ASTFrontendAction
	{
	public:
		explicit
		ReflectionAction(ReflectionContext& a_reflectionContext, OnHandleSourceFileFn* a_callback)
			: m_reflectionContext(a_reflectionContext), m_callback(a_callback) {}

		std::unique_ptr<clang::ASTConsumer>
		CreateASTConsumer(
			clang::CompilerInstance& CI,
			llvm::StringRef InFile
		) override
		{
			(void) InFile;

			m_reflectionContext.Init(CI);

			auto& opts = CI.getDiagnosticOpts();
			opts.VerifyDiagnostics = false;
			opts.IgnoreWarnings = true;
			opts.ShowCarets = false;
			return std::make_unique<ReflectionConsumer>(CI, m_reflectionContext, m_callback);
		}

	private:
		ReflectionContext& m_reflectionContext;
		OnHandleSourceFileFn* m_callback;
	};

	class ReflectionActionFactory : public clang::tooling::FrontendActionFactory
	{
	public:
		explicit
		ReflectionActionFactory(ReflectionContext* a_reflectionContext, OnHandleSourceFileFn* a_callback)
			: m_reflectionContext(*a_reflectionContext), m_callback(a_callback) {}

		std::unique_ptr<clang::FrontendAction>
		create() override
		{
			return std::make_unique<ReflectionAction>(m_reflectionContext, m_callback);
		}

	private:
		ReflectionContext& m_reflectionContext;
		OnHandleSourceFileFn* m_callback;
	};
}
