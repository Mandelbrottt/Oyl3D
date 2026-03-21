#pragma once

namespace Spyll
{
	class ReflectionGenerator;

	class DeclVisitor final : public clang::RecursiveASTVisitor<DeclVisitor>
	{
	public:
		explicit
		DeclVisitor(clang::SourceManager& SM);

		~DeclVisitor();

		bool
		VisitCXXRecordDecl(clang::CXXRecordDecl* Decl);

		bool
		VisitEnumDecl(clang::EnumDecl* Decl);

		bool
		VisitFunctionDecl(clang::FunctionDecl* Decl);

		clang::SourceManager*
		GetSourceManager() const;

		void
		SetContext(clang::ASTContext* Ctx);

	private:
		std::string
		GetDeclLocation(clang::SourceLocation Loc) const;

		void
		PrintDecl(const clang::NamedDecl* NamedDecl) const;

		template<typename DeclT>
		void
		VisitDeclCommon(DeclT* Decl);

		std::unique_ptr<ReflectionGenerator> Generator;

		clang::SourceManager& SourceManager;
		clang::ASTContext* Context = nullptr;
	};
}
