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

		if (auto ctx = llvm::dyn_cast<clang::CXXRecordDecl>(Decl->getDeclContext()))
		{
			return ShouldReflectDecl(ctx);
		}

		return false;
	}

	bool
	ReflectionParser::VisitCXXRecordDecl(clang::CXXRecordDecl* Decl)
	{
		if (!Decl->isCompleteDefinition())
			return true;

		if (!ShouldReflectDecl(Decl))
			return true;

		m_classes.emplace_back(new Class(Decl));

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

		if (!ShouldReflectDecl(Decl))
			return true;

		m_functions.emplace_back(new Function(Decl));

		return true;
	}

	bool
	ReflectionParser::VisitVarDecl(clang::VarDecl* Decl)
	{
		if (Decl->getDeclContext()->isRecord())
			return true;

		if (!Decl->isCanonicalDecl())
			return true;

		if (!Decl->getStorageClass() == clang::SC_Extern)
			return true;

		if (!ShouldReflectDecl(Decl))
			return true;

		m_globals.emplace_back(new Variable(Decl));

		return true;
	}
	
	bool
	ReflectionParser::VisitEnumDecl(clang::EnumDecl* Decl)
	{
		if (!Decl->isCompleteDefinition())
			return true;

		if (!ShouldReflectDecl(Decl))
			return true;

		m_enums.emplace_back(new Enum(Decl));

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
}
