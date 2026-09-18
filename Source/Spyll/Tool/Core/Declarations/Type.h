#pragma once

#include "Declaration.h"

namespace clang
{
	class TypeDecl;
}

class Test
{
	struct Foo {};

public:

};

// std::unordered_map<int, Test::Foo>
// Type: std::unordered_map
// TemplateParams:
//   int
//   Test::Foo

namespace Spyll
{
	class TemplateParam : public Declaration
	{
		friend class ReflectionParser;

	public:
		explicit
		TemplateParam(const clang::NamedDecl* a_decl);

		Declaration*
		GetDeclaration() const { return m_declaration; }

		bool
		ShouldReflect() const override;

	private:
		// Set by ReflectionParser
		Declaration* m_declaration = nullptr;
	};

	class Type : public Declaration
	{
		friend class ReflectionParser;

	public:
		explicit
		Type(const clang::TypeDecl* a_decl);

		bool
		ShouldReflect() const override;

		size_t
		GetSize() const { return m_size; }

		size_t
		GetAlignment() const { return m_alignment; }

		const std::vector<TemplateParam>&
		GetTemplateParams() const { return m_templateParams; }

		const clang::TypeDecl*
		GetClangDecl() const;

	protected:
		size_t m_size;
		size_t m_alignment;

		std::vector<TemplateParam> m_templateParams;
	};
}
