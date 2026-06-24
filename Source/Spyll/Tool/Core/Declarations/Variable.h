#pragma once

#include "Declaration.h"

namespace clang
{
	class VarDecl;
}

namespace Spyll
{
	class Class;

	class Variable : public Declaration
	{
	public:
		explicit
		Variable(const clang::VarDecl* a_decl, Class* a_parent, clang::SourceManager* a_sourceManager);

		virtual
		~Variable();

		Class*
		GetParent() const;

		int
		GetAccessSpecifier() const;

		std::string_view
		GetTypeAsString() const;

	private:
		std::string m_type;

		Class* m_parent;

		int m_accessSpecifier;
	};
}
