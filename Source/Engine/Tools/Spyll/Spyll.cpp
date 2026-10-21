#include "Spyll.h"

namespace Spyll
{
	static std::string_view REFLECT_ANNOTATION = "__REFLECT__";

	bool
	ReflectionVisitor::ShouldReflectDecl(clang::NamedDecl* Decl) const
	{
		auto loc = Decl->getLocation();
		if (m_sourceManager.isInSystemHeader(loc) || m_sourceManager.isInExternCSystemHeader(loc))
			return false;

		// Reflect if Decl has annotate("__REFLECT__") attribute
		for (const auto* iter : Decl->specific_attrs<clang::AnnotateAttr>())
		{
			// __attribute__((annotate("__REFLECT__")))
			const clang::AnnotateAttr* attr = clang::dyn_cast<clang::AnnotateAttr>(iter);
			llvm::StringRef annotation = attr->getAnnotation();
			if (annotation.compare(REFLECT_ANNOTATION) == 0)
			{
				return true;
			}
		}

		// Any class with a base ancestor of:
		// - Oyl::Module
		// - Oyl::Reflection::Attribute
		// Should be reflected
		if (auto record = llvm::dyn_cast<clang::CXXRecordDecl>(Decl))
		{
			// TODO: Don't hardcode
			if (record->getQualifiedNameAsString() == "Oyl::Reflection::Attribute")
			{
				return true;
			}
			if (record->getQualifiedNameAsString() == "Oyl::Module")
			{
				return true;
			}

			for (auto base : record->bases())
			{
				auto baseRecordDecl = base.getType().getCanonicalType()->getAsCXXRecordDecl();
				if (baseRecordDecl && ShouldReflectDecl(baseRecordDecl))
				{
					return true;
				}
			}
		}

		if (auto ctx = llvm::dyn_cast<clang::CXXRecordDecl>(Decl->getDeclContext()))
		{
			return ShouldReflectDecl(ctx);
		}

		return false;
	}

	bool
	ReflectionVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl* Decl)
	{
		if (!Decl->isCompleteDefinition())
			return true;

		// Don't parse unspecialized template types
		if (Decl->isDependentType())
			return true;

		if (!ShouldReflectDecl(Decl))
			return true;

		m_reflectionContext.classes.emplace_back(Decl);

		return true;
	}

	bool
	ReflectionVisitor::VisitFunctionDecl(clang::FunctionDecl* Decl)
	{
		if (Decl->getDeclContext()->isRecord())
			return true;

		if (!Decl->isCanonicalDecl())
			return true;

		if (Decl->isDependentContext())
			return true;

		if (Decl->getStorageClass() != clang::SC_Extern)
			return true;

		if (!ShouldReflectDecl(Decl))
			return true;

		m_reflectionContext.globalFunctions.emplace_back(Decl);

		return true;
	}

	bool
	ReflectionVisitor::VisitVarDecl(clang::VarDecl* Decl)
	{
		if (Decl->getDeclContext()->isRecord())
			return true;

		if (!Decl->isCanonicalDecl())
			return true;

		if (Decl->getStorageClass() != clang::SC_Extern)
			return true;

		if (!ShouldReflectDecl(Decl))
			return true;

		m_reflectionContext.globalVariables.emplace_back(Decl);

		return true;
	}

	bool
	ReflectionVisitor::VisitEnumDecl(clang::EnumDecl* Decl)
	{
		if (!Decl->isCompleteDefinition())
			return true;

		if (!ShouldReflectDecl(Decl))
			return true;

		m_reflectionContext.enums.emplace_back(Decl);

		return true;
	}

	std::string
	ReflectionVisitor::GetDeclLocation(clang::SourceLocation Loc) const
	{
		(void) Loc;
		return {};
	}

	void
	ReflectionVisitor::PrintDecl(clang::NamedDecl* NamedDecl) const
	{
		(void) NamedDecl;
	}
}
