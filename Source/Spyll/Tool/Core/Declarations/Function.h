#pragma once

#include <vector>

#include "Argument.h"
#include "Declaration.h"

namespace clang
{
	class FunctionDecl;
}

namespace Spyll
{
	class Type;

	class Function : public Declaration
	{
		friend class ReflectionParser;

	public:
		explicit
		Function(const clang::FunctionDecl* a_decl, Type* a_parent = nullptr);

		bool
		ShouldReflect() const override;

		bool
		IsDeleted() const;

		const Type*
		GetReturnType() const { return m_returnType; }

		std::string_view
		GetReturnTypeAsString() const { return m_returnTypeAsString; }

		const std::vector<Argument>&
		GetArguments() const { return m_arguments; }

		const clang::FunctionDecl*
		GetClangDecl() const;

	private:
		Type* m_returnType = nullptr;
		std::string m_returnTypeAsString;

		std::vector<Argument> m_arguments;
	};
}
