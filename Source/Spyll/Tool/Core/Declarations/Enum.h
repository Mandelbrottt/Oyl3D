#pragma once

#include "Declaration.h"

namespace clang
{
	class EnumDecl;
}

namespace Spyll
{
	class Type;

	class Enum : public Declaration
	{
	public:
		explicit
		Enum(const clang::EnumDecl* a_decl, clang::SourceManager* a_sourceManager);

		virtual
		~Enum();

	public:
		struct Entry
		{
			// TODO: Add Attribute support
			std::string identifier;
			int64_t value;
		};

		std::string_view
		GetUnderlyingTypeAsString() const
		{
			return m_underlyingType;
		}

		const std::vector<Entry>&
		GetEntries() const
		{
			return m_entries;
		}

	private:
		std::string m_underlyingType;

		std::vector<Entry> m_entries;
	};
}
