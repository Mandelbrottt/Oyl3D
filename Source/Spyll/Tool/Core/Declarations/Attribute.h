#pragma once

#include <string>
#include <vector>

namespace clang
{
	class Decl;
}

namespace Spyll
{
	struct Attribute
	{
		std::string type;
		std::string initializer;
	};

	class AttributeParser
	{
	public:
		explicit
		AttributeParser(const clang::Decl* a_decl);

		const std::vector<Attribute>&
		GetAttributes() const
		{
			return m_attributes;
		}

	private:
		std::vector<Attribute> m_attributes;
	};
}