#include "ReflectionParseAction.h"

namespace Spyll
{
	class ReflectionParserVisitor final : public clang::RecursiveASTVisitor<ReflectionParserVisitor>
	{
	public:
		explicit
		ReflectionParserVisitor(ReflectionParser* Parser);

		~ReflectionParserVisitor();

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
		ReflectionParser* Parser;

		clang::SourceManager* SourceManager = nullptr;
		clang::ASTContext* Context = nullptr;
		clang::DiagnosticOptions* DiagnosticOptions = nullptr;
	};
	
	class ReflectionParserConsumer final : public clang::ASTConsumer
	{
	public:
		explicit
		ReflectionParserConsumer(clang::CompilerInstance* CI, ReflectionParser* Parser);

		void
		HandleTranslationUnit(clang::ASTContext& Ctx) override;

	private:
		ReflectionParserVisitor Visitor;
	};

	ReflectionParseAction::ReflectionParseAction(ReflectionParserOptions* a_options)
		: m_options(a_options) {}

	std::unique_ptr<clang::ASTConsumer>
	ReflectionParseAction::CreateASTConsumer(
		clang::CompilerInstance& CI,
		llvm::StringRef InFile
	)
	{
		(void) InFile;

		CI.getDiagnosticOpts().IgnoreWarnings = true;

		return std::make_unique<ReflectionParserConsumer>(&CI, &Parser);
	}

	void
	ReflectionParseAction::EndSourceFileAction()
	{
		if (m_options->onSourceParsedCallback)
		{
			auto onSourceParsedCallback = m_options->onSourceParsedCallback;
			onSourceParsedCallback(&Parser);
		}
	}

	ReflectionParserConsumer::ReflectionParserConsumer(clang::CompilerInstance* CI, ReflectionParser* Parser)
		: Visitor(Parser)
	{
		Visitor.SetDiagnosticOptions(&CI->getDiagnosticOpts());
		Visitor.SetSourceManager(&CI->getSourceManager());
	}

	void
	ReflectionParserConsumer::HandleTranslationUnit(clang::ASTContext& Ctx)
	{
		auto printingPolicy = Ctx.getPrintingPolicy();
		printingPolicy.Bool = true;
		printingPolicy.FullyQualifiedName = true;
		printingPolicy.SuppressScope = false;
		printingPolicy.SuppressUnwrittenScope = false;
		Ctx.setPrintingPolicy(printingPolicy);
		
		Visitor.SetContext(&Ctx);
		Visitor.TraverseDecl(Ctx.getTranslationUnitDecl());
	}

	ReflectionParserVisitor::ReflectionParserVisitor(ReflectionParser* Parser)
		: Parser(Parser) {}

	ReflectionParserVisitor::~ReflectionParserVisitor() {}

	bool
	ReflectionParserVisitor::ShouldReflectDecl(const clang::NamedDecl* Decl) const
	{
		auto loc = Decl->getLocation();
		if (SourceManager->isInSystemHeader(loc) || SourceManager->isInExternCSystemHeader(loc))
			return false;

		return Parser->ShouldReflectDecl(Decl);
	}

	bool
	ReflectionParserVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl* Decl)
	{
		return Parser->ParseCXXRecordDecl(Decl, SourceManager);
	}

	bool
	ReflectionParserVisitor::VisitFunctionDecl(clang::FunctionDecl* Decl)
	{
		return Parser->ParseFunctionDecl(Decl, SourceManager);
	}

	bool
	ReflectionParserVisitor::VisitVarDecl(clang::VarDecl* Decl)
	{
		return Parser->ParseVarDecl(Decl, SourceManager);
	}

	bool
	ReflectionParserVisitor::VisitEnumDecl(clang::EnumDecl* Decl)
	{
		return Parser->ParseEnumDecl(Decl, SourceManager);
	}

	clang::SourceManager*
	ReflectionParserVisitor::GetSourceManager() const
	{
		return SourceManager;
	}

	void
	ReflectionParserVisitor::SetSourceManager(clang::SourceManager* SM)
	{
		SourceManager = SM;
	}

	clang::ASTContext*
	ReflectionParserVisitor::GetContext() const
	{
		return Context;
	}

	void
	ReflectionParserVisitor::SetContext(clang::ASTContext* Ctx)
	{
		Context = Ctx;
	}

	clang::DiagnosticOptions*
	ReflectionParserVisitor::GetDiagnosticOptions() const
	{
		return DiagnosticOptions;
	}

	void
	ReflectionParserVisitor::SetDiagnosticOptions(clang::DiagnosticOptions* DO)
	{
		DiagnosticOptions = DO;
	}

	std::string
	ReflectionParserVisitor::GetDeclLocation(clang::SourceLocation Loc) const
	{
		return Loc.printToString(*SourceManager);
	}

	void
	ReflectionParserVisitor::PrintDecl(const clang::NamedDecl* NamedDecl) const
	{
		(void) NamedDecl;
	}
}
