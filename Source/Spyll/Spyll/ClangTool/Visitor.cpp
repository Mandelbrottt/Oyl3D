#include "pch.h"
#include "Visitor.h"

#include "ReflectionGenerator.h"

namespace Spyll
{
	DeclVisitor::DeclVisitor(clang::SourceManager& SM, ReflectionGenerator* Generator)
		: SourceManager(SM), Generator(Generator) {}

	DeclVisitor::~DeclVisitor() {}
	
	template<typename DeclT>
	void
	DeclVisitor::VisitDeclCommon(DeclT* Decl)
	{
		Generator->ScrapeDecl(Decl);

		if (Generator->ShouldReflectDecl(Decl))
		{
			PrintDecl(Decl);
		}
	}

	bool
	DeclVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl* Decl)
	{
		if (!Decl->hasDefinition() || !Decl->getIdentifier())
			return true;

		if (!Generator->ShouldReflectDecl(Decl))
			return true;
		
		VisitDeclCommon(Decl);
		return true;
	}

	bool
	DeclVisitor::VisitEnumDecl(clang::EnumDecl* Decl)
	{
		if (!Decl->isComplete() || !Decl->getIdentifier())
			return true;
		
		if (!Generator->ShouldReflectDecl(Decl))
			return true;
		
		VisitDeclCommon(Decl);
		return true;
	}

	bool
	DeclVisitor::VisitFunctionDecl(clang::FunctionDecl* Decl)
	{
		if (!Decl->getDefinition() || !Decl->getIdentifier())
			return true;
		
		if (!Generator->ShouldReflectDecl(Decl))
			return true;
		
		VisitDeclCommon(Decl);
		return true;
	}

	bool
	DeclVisitor::VisitVarDecl(clang::VarDecl* Decl)
	{
		if (!Decl->getDefinition() || !Decl->getIdentifier())
			return true;

		if (!Generator->ShouldReflectDecl(Decl))
			return true;

		VisitDeclCommon(Decl);
		return true;
	}

	clang::SourceManager*
	DeclVisitor::GetSourceManager() const
	{
		return &SourceManager;
	}

	void
	DeclVisitor::SetContext(clang::ASTContext* Ctx)
	{
		Context = Ctx;
		Generator->SetContext(Ctx);
	}

	std::string
	DeclVisitor::GetDeclLocation(clang::SourceLocation Loc) const
	{
		return Loc.printToString(SourceManager);
	}

	void
	DeclVisitor::PrintDecl(const clang::NamedDecl* NamedDecl) const
	{
		(void) NamedDecl;
	}
}
