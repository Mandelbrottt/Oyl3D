#include "Record.h"

namespace Spyll
{
	Record::Record(clang::RecordDecl* a_decl)
		: Type(a_decl)
	{
		for (const auto* field : a_decl->fields())
		{
			m_fields.emplace_back(field);
		}
	}

	bool
	Record::ShouldReflect() const
	{
		return Type::ShouldReflect();
	}

	const clang::RecordDecl*
	Record::GetClangDecl() const
	{
		return clang::dyn_cast<clang::RecordDecl>(Type::GetClangDecl());
	}
}
