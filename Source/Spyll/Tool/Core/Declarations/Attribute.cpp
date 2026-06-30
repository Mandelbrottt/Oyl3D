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

			if (category.compare("__ATTR__") != 0)
			{
				continue;
			}

			for (auto arg : annotation->args())
			{
				if (auto constExpr = clang::dyn_cast<clang::ConstantExpr>(arg))
				{
					Attribute attribute;

					clang::Expr::EvalResult result;
					constExpr->EvaluateAsConstantExpr(result, ctx);

					auto desugaredType = constExpr->getType().getDesugaredType(ctx);
					attribute.type = desugaredType.getAsString(ctx.getPrintingPolicy());

					int i = 0;
					for (const auto* field : desugaredType->getAsRecordDecl()->fields())
					{
						auto fieldTypeDecl = field->getType()->getAsTagDecl();
						if (fieldTypeDecl && fieldTypeDecl->getQualifiedNameAsString() == "Oyl::Reflection::TypeId")
						{
							// Assume users will call a function with signature template<typename T> TypeId(*)()
							clang::Expr* expr = constExpr->getSubExpr();
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
							auto tempArg = callee->getTemplateSpecializationArgs()->get(0);

							auto recordDecl = tempArg.getAsType()->getAsRecordDecl();
							attribute.arguments.emplace_back("__TypeId " + recordDecl->getQualifiedNameAsString());
						} else
						{
							auto apValue = result.Val.getStructField(i);
							auto str = apValue.getAsString(ctx, field->getType());
							attribute.arguments.emplace_back(std::move(str));
						}

						i++;
					}

					m_attributes.emplace_back(std::move(attribute));
				}
			}
		}
	}
}
