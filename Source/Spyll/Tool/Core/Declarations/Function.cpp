#include "Function.h"

#include <clang/AST/QualTypeNames.h>

namespace Spyll
{
	Function::Function(const clang::FunctionDecl* a_decl, Type* a_parent)
		: Declaration(a_decl, a_parent)
	{
		auto& ctx = a_decl->getASTContext();
		auto qualifiedType = a_decl->getType();
		auto printingPolicy = ctx.getPrintingPolicy();
		m_returnTypeAsString = clang::TypeName::getFullyQualifiedName(qualifiedType, ctx, printingPolicy);

		for (auto* argDecl : a_decl->parameters())
		{
			m_arguments.emplace_back(argDecl);
		}
	}

	bool
	Function::ShouldReflect() const
	{
		if (!Declaration::ShouldReflect())
		{
			return false;
		}

		auto* functionDecl = GetClangDecl();

		// Member Operators are technically not functions, they are functors
		// Similar to lambdas or member function pointers. We can't take the address of these functions
		// TODO: Add thunk lambda call as functionPtr
		if (functionDecl->isOverloadedOperator())
			return false;

		//if (!IsTypeOfDeclVisible(functionDecl))
		//	return false;

		//for (auto paramDecl : functionDecl->parameters())
		//{
		//	if (!IsTypeOfDeclVisible(paramDecl))
		//		return false;
		//}

		return true;
	}

	bool
	Function::IsDeleted() const
	{
		return GetClangDecl()->isDeleted();
	}

	const clang::FunctionDecl*
	Function::GetClangDecl() const
	{
		return clang::dyn_cast<clang::FunctionDecl>(Declaration::GetClangDecl());
	}
}
