#include "Function.h"

#include <clang/AST/QualTypeNames.h>

namespace Spyll
{
	Function::Function(const clang::FunctionDecl* a_decl, Type* a_parent)
		: Declaration(a_decl), m_parent(a_parent)
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

	bool
	Function::ShouldReflect() const
	{
		if (!Declaration::ShouldReflect())
		{
			return false;
		}

		auto functionDecl = clang::dyn_cast<clang::FunctionDecl>(m_decl);

		// Member Operators are technically not functions, they are functors
		// Similar to lambdas or member function pointers. We can't take the address of these functions
		// TODO: Add thunk lambda call as functionPtr
		if (functionDecl->isOverloadedOperator())
			return false;

		auto type = functionDecl->getType().getTypePtr();
		while (type->isPointerType())
		{
			type = type->getPointeeOrArrayElementType();
		}
		auto returnTypeTagDecl = type->getAsRecordDecl();
		if (returnTypeTagDecl
			&& (returnTypeTagDecl->getAccess() == clang::AS_protected
				|| returnTypeTagDecl->getAccess() == clang::AS_private))
		{
			return false;
		}

		for (auto arg : functionDecl->parameters())
		{
			type = arg->getType().getTypePtr();
			while (type->isPointerType())
			{
				type = type->getPointeeOrArrayElementType();
			}
			auto typeTagDecl = type->getAsRecordDecl();
			if (typeTagDecl &&
				(typeTagDecl->getAccess() == clang::AS_protected
				 || typeTagDecl->getAccess() == clang::AS_private))
			{
				return false;
			}
		}

		return true;
	}

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
