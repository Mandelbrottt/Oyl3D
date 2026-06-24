#pragma once

#include "Declaration.h"

namespace clang
{
	class EnumDecl;
}

namespace Spyll
{
	class Class;

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
			std::string name;
			int64_t value;
		};

	private:
		std::string m_underlyingType;

		std::vector<Entry> m_entries;
	};
}
