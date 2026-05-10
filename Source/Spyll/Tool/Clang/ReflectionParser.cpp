#include "ReflectionParser.h"

namespace Spyll
{
	constexpr const char* REFLECT_ANNOTATION = "__REFLECT__";

	ReflectionParser::ReflectionParser() {}

	ReflectionParser::~ReflectionParser() {}

	bool
	ReflectionParser::ShouldReflectDecl(const clang::NamedDecl* Decl) const
	{
		// Reflect if Decl has annotate("__REFLECT__") attribute
		for (const auto* iter : Decl->specific_attrs<clang::AnnotateAttr>())
		{
			// __attribute__((annotate("__REFLECT__")))
			const clang::AnnotateAttr* attr = clang::dyn_cast<clang::AnnotateAttr>(iter);
			llvm::StringRef annotation = attr->getAnnotation();
			if (annotation.compare(REFLECT_ANNOTATION) == 0)
			{
				return true;
			}
		}

		return false;
	}

	bool
	ReflectionParser::VisitCXXRecordDecl(clang::CXXRecordDecl* Decl)
	{
		if (!Decl->isCompleteDefinition())
			return true;

		Parse(Decl);

		return true;
	}

	bool
	ReflectionParser::VisitFunctionDecl(clang::FunctionDecl* Decl)
	{
		if (!Decl->isGlobal())
			return true;

		if (!Decl->isCanonicalDecl())
			return true;

		if (!Decl->getStorageClass() == clang::SC_Extern)
			return true;

		Parse(Decl);

		return true;
	}

	bool
	ReflectionParser::VisitVarDecl(clang::VarDecl* Decl)
	{
		if (!Decl->isCanonicalDecl())
			return true;

		if (!Decl->getStorageClass() == clang::SC_Extern)
			return true;

		Parse(Decl);

		return true;
	}
	
	bool
	ReflectionParser::VisitEnumDecl(clang::EnumDecl* Decl)
	{
		if (!Decl->isCompleteDefinition())
			return true;

		Parse(Decl);

		return true;
	}

	clang::SourceManager*
	ReflectionParser::GetSourceManager() const
	{
		return SourceManager;
	}

	void
	ReflectionParser::SetSourceManager(clang::SourceManager* SM)
	{
		SourceManager = SM;
	}

	clang::ASTContext*
	ReflectionParser::GetContext() const
	{
		return Context;
	}

	void
	ReflectionParser::SetContext(clang::ASTContext* Ctx)
	{
		Context = Ctx;
	}

	clang::DiagnosticOptions*
	ReflectionParser::GetDiagnosticOptions() const
	{
		return DiagnosticOptions;
	}

	void
	ReflectionParser::SetDiagnosticOptions(clang::DiagnosticOptions* DO)
	{
		DiagnosticOptions = DO;
	}

	std::string
	ReflectionParser::GetDeclLocation(clang::SourceLocation Loc) const
	{
		return Loc.printToString(*SourceManager);
	}

	void
	ReflectionParser::PrintDecl(const clang::NamedDecl* NamedDecl) const
	{
		(void) NamedDecl;
	}

	ReflectionParserConsumer::ReflectionParserConsumer(clang::SourceManager& SM, ReflectionParser* Parser)
		: Parser(Parser)
	{
		Parser->SetSourceManager(&SM);
	}

	void
	ReflectionParserConsumer::HandleTranslationUnit(clang::ASTContext& Ctx)
	{
		Parser->SetContext(&Ctx);
		Parser->TraverseDecl(Ctx.getTranslationUnitDecl());
	}

	ReflectionParserAction::ReflectionParserAction(ReflectionParser* Parser)
		: Parser(Parser) {}

	std::unique_ptr<clang::ASTConsumer>
	ReflectionParserAction::CreateASTConsumer(
		clang::CompilerInstance& CI,
		llvm::StringRef InFile
	)
	{
		(void) InFile;

		CI.getDiagnosticOpts().IgnoreWarnings = true;
		
		Parser->SetDiagnosticOptions(&CI.getDiagnosticOpts());

		return std::make_unique<ReflectionParserConsumer>(CI.getSourceManager(), Parser);
	}
}
