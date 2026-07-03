#include "Attribute.h"

#include <clang/AST/Decl.h>

namespace Spyll
{
	AttributeParser::AttributeParser(const clang::Decl* a_decl)
	{
		const auto& ctx = a_decl->getASTContext();

		for (auto annotation : a_decl->specific_attrs<clang::AnnotateAttr>())
		{
			auto category = annotation->getAnnotation();

			// Oyl Attributes must be under __ATTR__ category
			if (category.compare("__ATTR__") != 0)
			{
				continue;
			}

			// Iterate the arguments of the annotation
			for (auto arg : annotation->args())
			{
				// Annotation arguments are constexpr
				auto constExpr = clang::dyn_cast<clang::ConstantExpr>(arg);
				if (!constExpr)
				{
					continue;
				}

				Attribute attribute;

				// Get the constexpr expression that represents this attribute
				clang::Expr::EvalResult result;
				constExpr->EvaluateAsConstantExpr(result, ctx);

				auto desugaredType = constExpr->getType().getDesugaredType(ctx);
				attribute.type = desugaredType.getAsString(ctx.getPrintingPolicy());

				// Iterate the fields of the AttributeType
				int i = 0;
				for (const auto* field : desugaredType->getAsRecordDecl()->fields())
				{
					auto fieldTypeDecl = field->getType()->getAsTagDecl();

					// Special case for TypeId
					std::string argument;
					if (fieldTypeDecl && fieldTypeDecl->getQualifiedNameAsString() == "Oyl::Reflection::TypeId")
					{
						if (GetTypeNameFromFunctionExpr(constExpr, &argument))
						{
							attribute.arguments.emplace_back("__TypeId " + argument);
						}
					} else
					{ // Add result of constexpr as string to attribute args list
						auto apValue = result.Val.getStructField(i);
						argument = apValue.getAsString(ctx, field->getType());
						attribute.arguments.emplace_back(std::move(argument));
					}

					i++;
				}

				m_attributes.emplace_back(std::move(attribute));
			}
		}
	}

	bool
	AttributeParser::GetTypeNameFromFunctionExpr(
		clang::ConstantExpr* a_expr,
		std::string* a_outString
	)
	{
		if (!a_expr)
		{
			return false;
		}

		// Check for function with one of the following signatures
		// - template<typename T> TypeId(*)(void)
		// - static TypeId(T::*)(void)
		clang::Expr* expr = a_expr->getSubExpr();
		while (expr->getStmtClass() != clang::Stmt::CXXConstructExprClass)
		{
			if (auto ptr = clang::dyn_cast<clang::CastExpr>(expr))
				expr = ptr->getSubExpr();
			if (auto ptr = clang::dyn_cast<clang::FullExpr>(expr))
				expr = ptr->getSubExpr();
			if (auto ptr = clang::dyn_cast<clang::CXXBindTemporaryExpr>(expr))
				expr = ptr->getSubExpr();
		}
		auto cxxConstruct = clang::dyn_cast<clang::CXXConstructExpr>(expr);
		auto call = clang::dyn_cast<clang::CallExpr>(cxxConstruct->getArg(0));
		auto callee = clang::dyn_cast<clang::FunctionDecl>(call->getCalleeDecl());

		if (callee->isFunctionTemplateSpecialization())
		{
			auto tempArg = callee->getTemplateSpecializationArgs()->get(0);
			auto recordDecl = tempArg.getAsType()->getAsRecordDecl();
			*a_outString = recordDecl->getQualifiedNameAsString();
			return true;
		}

		return false;
	}
}
