#include "Enum.h"

namespace Spyll
{
	Enum::Enum(const clang::EnumDecl* a_decl)
		: Declaration(a_decl)
	{
		m_underlyingType = a_decl->getIntegerType().getUnqualifiedType().getAsString();

		for (auto* entryDecl : a_decl->enumerators())
		{
			m_entries.emplace_back(Entry {
				entryDecl->getNameAsString(),
				entryDecl->getInitVal().getExtValue()
			});
		}
	}

	Enum::~Enum() {}
}
