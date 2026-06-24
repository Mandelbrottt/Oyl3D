#pragma once

#include <string>

namespace clang
{
	class NamedDecl;
}

namespace Spyll
{
	class Declaration
	{
	protected:
		explicit
		Declaration(const clang::NamedDecl* a_decl);

		virtual
		~Declaration();

	public:
		virtual
		bool
		ShouldReflect() const;

		void*
		GetSourceLocationEncoding() const;

	protected:
		// Attributes / Metadata

		bool m_enabled;

		std::string m_name;
		std::string m_qualifiedName;

	private:
		void* m_sourceLocationEncoding;
	};
}