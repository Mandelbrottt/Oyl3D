#pragma once

#include "Declaration.h"

namespace clang
{
	class TypeDecl;
	class VarDecl;
}

namespace Spyll
{
	class Type;

	class Variable : public Declaration
	{
		friend class ReflectionParser;

	public:
		explicit
		Variable(const clang::VarDecl* a_decl, Type* a_parent = nullptr);

		bool
		ShouldReflect() const override;

		const Type*
		GetType() const { return m_type; }

		std::string_view
		GetTypeAsString() const { return m_typeAsString; }

		const clang::VarDecl*
		GetClangDecl() const;

	private:
		Type* m_type = nullptr;
		std::string m_typeAsString;
	};
}
