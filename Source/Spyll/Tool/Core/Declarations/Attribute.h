#pragma once

#include <string>
#include <vector>

namespace clang
{
	class Decl;
	class ConstantExpr;
}

namespace Spyll
{
	struct Attribute
	{
		std::string type;
		std::vector<std::string> arguments;
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
		static
		bool
		GetTypeNameFromFunctionExpr(clang::ConstantExpr* a_expr, std::string* a_outString);

	private:
		std::vector<Attribute> m_attributes;
	};
}