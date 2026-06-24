#pragma once

#include <string>

namespace clang
{
	class NamedDecl;
	class SourceManager;
}

namespace Spyll
{
	class Declaration
	{
		friend class ReflectionParserVisitor;

	protected:
		explicit
		Declaration(const clang::NamedDecl* a_decl, const clang::SourceManager* a_sourceManager);

		virtual
		~Declaration();

	public:
		virtual
		bool
		ShouldReflect() const;

		std::string_view
		GetName() const;

		std::string_view
		GetQualifiedName() const;

		std::string_view
		GetSourceFile() const;

		std::uint32_t
		GetSourceLine() const;

	protected:
		// Attributes / Metadata

		bool m_enabled;

		std::string m_name;
		std::string m_qualifiedName;

	private:
		std::string m_sourceFile;
		uint32_t m_sourceLine;
	};
}
