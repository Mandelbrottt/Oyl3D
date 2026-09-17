#pragma once

#include "Variable.h"

namespace clang
{
	class FieldDecl;
	class TypeDecl;
}

namespace Spyll
{
	class Type;

	class Field : public Declaration
	{
		friend class ReflectionParser;

	public:
		explicit
		Field(const clang::FieldDecl* a_decl);

		bool
		ShouldReflect() const override;

		const Type*
		GetType() const { return m_type; }

		std::string_view
		GetTypeAsString() const { return m_typeAsString; }

		size_t
		GetOffsetInBits() const { return m_offsetInBits; }

		bool
		IsConst() const { return m_isConst; }

		const clang::FieldDecl*
		GetClangDecl() const;

	private:
		Type* m_type = nullptr;
		std::string m_typeAsString;

		size_t m_offsetInBits;
		bool m_isConst;
	};
}
