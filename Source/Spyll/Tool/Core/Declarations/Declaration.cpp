#include "Declaration.h"

#include <filesystem>

#include <clang/AST/Decl.h>

namespace Spyll
{
	Declaration::Declaration(const clang::NamedDecl* a_decl, const clang::SourceManager* a_sourceManager)
	{
		llvm::raw_string_ostream out { m_name };
		a_decl->getDeclName().print(out, a_decl->getASTContext().getPrintingPolicy());

		m_qualifiedName = a_decl->getQualifiedNameAsString();

		m_enabled = true;

		auto sourceLocation = a_decl->getLocation();

		m_sourceFile = a_sourceManager->getFilename(sourceLocation);
		m_sourceFile = std::filesystem::path(m_sourceFile).lexically_normal().string();

		m_sourceLine = a_sourceManager->getSpellingLineNumber(sourceLocation);
	}

	Declaration::~Declaration() {}

	bool
	Declaration::ShouldReflect() const
	{
		return m_enabled;
	}

	std::string_view
	Declaration::GetName() const
	{
		return m_name;
	}

	std::string_view
	Declaration::GetQualifiedName() const
	{
		return m_qualifiedName;
	}

	std::string_view
	Declaration::GetSourceFile() const
	{
		return m_sourceFile;
	}

	std::uint32_t
	Declaration::GetSourceLine() const
	{
		return m_sourceLine;
	}
}
