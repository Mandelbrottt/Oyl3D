#pragma once

#include "Declaration.h"

namespace clang
{
	class Type;
}

namespace Spyll
{
	class Type : public Declaration
	{
	public:
		explicit
		Type(const clang::Type* a_type);

		virtual
		~Type();

	private:
		size_t m_size;
		size_t m_alignment;
	};
}
#pragma once
