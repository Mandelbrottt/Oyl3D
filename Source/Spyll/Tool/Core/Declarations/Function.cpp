#include "Function.h"

#include <clang/AST/QualTypeNames.h>

namespace Spyll
{
	Function::Function(const clang::FunctionDecl* a_decl, Type* a_parent, clang::SourceManager* a_sourceManager)
		: Declaration(a_decl, a_sourceManager), m_parent(a_parent)
	{
		m_accessSpecifier = a_decl->getAccess();

		auto& ctx = a_decl->getASTContext();
		auto printingPolicy = ctx.getPrintingPolicy();
		auto qualifiedType = a_decl->getReturnType();
		m_returnType = clang::TypeName::getFullyQualifiedName(qualifiedType, ctx, printingPolicy);

		for (auto* argDecl : a_decl->parameters())
		{
			auto argType = argDecl->getType();

			m_arguments.emplace_back(Argument {
				clang::TypeName::getFullyQualifiedName(argType, ctx, printingPolicy),
				argDecl->getNameAsString()
			});
		}
	}

	Function::~Function() {}

	Type*
	Function::GetParent() const
	{
		return m_parent;
	}

	int
	Function::GetAccessSpecifier() const
	{
		return m_accessSpecifier;
	}

	std::string_view
	Function::GetReturnTypeAsString() const
	{
		return m_returnType;
	}

	const std::vector<Argument>&
	Function::GetArguments() const
	{
		return m_arguments;
	}
}
