#pragma once
#pragma once
#pragma once

#include "Field.h"
#include "Type.h"

namespace clang
{
	class RecordDecl;
}

namespace Spyll
{
	class Record : public Type
	{
		friend class ReflectionParser;

	public:
		explicit
		Record(const clang::RecordDecl* a_decl);

		bool
		ShouldReflect() const override;

		const std::vector<Field>&
		GetFields() const { return m_fields; }

		const clang::RecordDecl*
		GetClangDecl() const;

	private:
		std::vector<Field> m_fields;
	};
}
