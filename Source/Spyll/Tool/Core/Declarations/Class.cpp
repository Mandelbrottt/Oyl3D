#include "Class.h"

#include <clang/AST/DeclCXX.h>

namespace Spyll
{
	Class::Class(clang::CXXRecordDecl* a_decl, clang::SourceManager* a_sourceManager)
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
			m_baseClasses.emplace_back(BaseClass { decl.getType().getAsString() });
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

	Class::~Class() {}

	bool
	Class::ShouldReflect() const
	{
		return Declaration::ShouldReflect();
	}

	size_t
	Class::GetSize() const
	{
		return m_size;
	}

	size_t
	Class::GetAlignment() const
	{
		return m_alignment;
	}

	const std::vector<BaseClass>&
	Class::GetBaseClasses() const
	{
		return m_baseClasses;
	}

	const std::vector<Field>&
	Class::GetFields() const
	{
		return m_fields;
	}

	const std::vector<Method>&
	Class::GetMethods() const
	{
		return m_methods;
	}

	const std::vector<Variable>&
	Class::GetVariables() const
	{
		return m_variables;
	}

	const std::vector<Function>&
	Class::GetFunctions() const
	{
		return m_functions;
	}
}
