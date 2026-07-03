#pragma once

#include "Declaration.h"

namespace clang
{
	class VarDecl;
}

namespace Spyll
{
	class Type;

	class Variable : public Declaration
	{
	public:
		explicit
		Variable(const clang::VarDecl* a_decl, Type* a_parent);

		virtual
		~Variable();

		bool
		ShouldReflect() const override;

		Type*
		GetParent() const;

		int
		GetAccessSpecifier() const;

		std::string_view
		GetTypeAsString() const;

	private:
		std::string m_type;

		Type* m_parent;

		int m_accessSpecifier;
	};
}
