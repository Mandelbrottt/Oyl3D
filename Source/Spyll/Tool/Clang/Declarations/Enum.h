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
		struct Entry
		{
			// TODO: Add Attribute support
			std::string name;
			int64_t value;
		};

		explicit
		Enum(const clang::EnumDecl* a_decl);

		virtual
		~Enum();

	private:
		std::string m_underlyingType;

		std::vector<Entry> m_entries;
	};
}
