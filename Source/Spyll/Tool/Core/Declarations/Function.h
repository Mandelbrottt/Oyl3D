#pragma once

#include "Declaration.h"

namespace clang
{
	class FunctionDecl;
}

namespace Spyll
{
	class Type;

	struct Argument
	{
		std::string type;
		std::string name;
	};

	class Function : public Declaration
	{
	public:
		explicit
		Function(const clang::FunctionDecl* a_decl, Type* a_parent, clang::SourceManager* a_sourceManager);

		virtual
		~Function();

		Type*
		GetParent() const;

		int
		GetAccessSpecifier() const;

		std::string_view
		GetReturnTypeAsString() const;

		const std::vector<Argument>&
		GetArguments() const;

	private:
		Type* m_parent;

		int m_accessSpecifier;

		std::string m_returnType;

		std::vector<Argument> m_arguments;
	};
}
