#pragma once

#include <string>

#include "Attribute.h"

namespace clang
{
	class NamedDecl;
	class ValueDecl;
	class SourceManager;
}

namespace Spyll
{
	class Class;

	extern
	bool
	IsTypeOfDeclVisible(const clang::ValueDecl* a_decl);

	enum class AccessSpecifier
	{
		Public,
		Protected,
		Private,
		None,
		Count
	};

	class Declaration
	{
		friend class ReflectionParserVisitor;
		friend class ReflectionParser;

	protected:
		explicit
		Declaration(const clang::NamedDecl* a_decl);

		virtual
		~Declaration() = default;

	public:
		virtual
		bool
		ShouldReflect() const;

		Class*
		GetParent() const { return m_parent; }

		std::string_view
		GetName() const { return m_name; }

		std::string_view
		GetQualifiedName() const { return m_qualifiedName; }

		AccessSpecifier
		GetAccessSpecifier() const;

		const std::vector<Attribute>&
		GetAttributes() const { return m_attributeParser.GetAttributes(); }

		const clang::NamedDecl*
		GetClangDecl() const;

		std::string_view
		GetSourceFile() const;

		std::uint32_t
		GetSourceLine() const;

	protected:
		virtual
		std::string
		ToString() const;

	protected:
		bool m_enabled;

		std::string m_name;
		std::string m_qualifiedName;

		AttributeParser m_attributeParser;

		Class* m_parent = nullptr;

	private:
		const clang::NamedDecl* m_decl = nullptr;

		std::string m_sourceFile;
		uint32_t m_sourceLine;
	};
}

namespace std
{
	inline
	std::string
	to_string(Spyll::AccessSpecifier a_accessSpecifier)
	{
		std::string_view strings[(size_t) Spyll::AccessSpecifier::Count];
		strings[(size_t) Spyll::AccessSpecifier::Public] = "Public";
		strings[(size_t) Spyll::AccessSpecifier::Protected] = "Protected";
		strings[(size_t) Spyll::AccessSpecifier::Private] = "Private";
		strings[(size_t) Spyll::AccessSpecifier::None] = "Global";
		return std::string(strings[(size_t) a_accessSpecifier]);
	}
}
