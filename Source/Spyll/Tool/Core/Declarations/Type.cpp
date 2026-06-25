#include "Type.h"

#include <clang/AST/DeclCXX.h>

namespace Spyll
{
	Type::Type(clang::CXXRecordDecl* a_decl, clang::SourceManager* a_sourceManager)
		: Declaration(a_decl, a_sourceManager)
	{
		auto* type = a_decl->getTypeForDecl();

		if (!a_decl->isAbstract())
		{
			const auto& ctx = a_decl->getASTContext();
			m_size = ctx.getTypeSizeInChars(type).getQuantity();
			m_alignment = ctx.getPreferredTypeAlignInChars(type->getCanonicalTypeUnqualified()).getQuantity();
		} else
		{
			m_size = 0;
			m_alignment = 0;
		}

		for (const auto& decl : a_decl->bases())
		{
			m_baseClasses.emplace_back(BaseType { decl.getType().getAsString() });
		}

		for (auto* decl : a_decl->decls())
		{
			switch (decl->getKind())
			{
				case clang::Decl::CXXConstructor:
				{
					break;
				}
				case clang::Decl::CXXDestructor:
				{
					break;
				}
				case clang::Decl::Field:
				{
					auto* fieldDecl = static_cast<clang::FieldDecl*>(decl);
					m_fields.emplace_back(fieldDecl, this, a_sourceManager);
					break;
				}
				case clang::Decl::CXXMethod:
				{
					auto* cxxMethodDecl = static_cast<clang::CXXMethodDecl*>(decl);

					// Member Operators are technically not functions, they are functors
					// Similar to lambdas or member function pointers. We can't take the address of these functions
					// TODO: Add thunk lambda call as functionPtr
					if (cxxMethodDecl->isOverloadedOperator())
						break;

					if (cxxMethodDecl->isInstance())
						m_methods.emplace_back(cxxMethodDecl, this, a_sourceManager);
					else
						m_functions.emplace_back(cxxMethodDecl, this, a_sourceManager);
					break;
				}
				case clang::Decl::Var:
				{
					auto* varDecl = static_cast<clang::VarDecl*>(decl);
					m_variables.emplace_back(varDecl, this, a_sourceManager);
					break;
				}
			}
		}
	}

	Type::~Type() {}

	bool
	Type::ShouldReflect() const
	{
		return Declaration::ShouldReflect();
	}

	size_t
	Type::GetSize() const
	{
		return m_size;
	}

	size_t
	Type::GetAlignment() const
	{
		return m_alignment;
	}

	const std::vector<BaseType>&
	Type::GetBaseClasses() const
	{
		return m_baseClasses;
	}

	const std::vector<Field>&
	Type::GetFields() const
	{
		return m_fields;
	}

	const std::vector<Method>&
	Type::GetMethods() const
	{
		return m_methods;
	}

	const std::vector<Variable>&
	Type::GetVariables() const
	{
		return m_variables;
	}

	const std::vector<Function>&
	Type::GetFunctions() const
	{
		return m_functions;
	}
}
