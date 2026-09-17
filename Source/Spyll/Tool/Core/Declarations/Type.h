#pragma once

#include "Declaration.h"

namespace clang
{
	class TypeDecl;
}

namespace Spyll
{
	class Type : public Declaration
	{
		friend class ReflectionParser;

	public:
		explicit
		Type(const clang::TypeDecl* a_decl);

	public:
		bool
		ShouldReflect() const override;

		size_t
		GetSize() const { return m_size; }

		size_t
		GetAlignment() const { return m_alignment; }

		const clang::TypeDecl*
		GetClangDecl() const;

	protected:
		size_t m_size;
		size_t m_alignment;
	};
}
