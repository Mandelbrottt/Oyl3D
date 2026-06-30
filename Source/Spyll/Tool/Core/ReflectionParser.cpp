#include "ReflectionParser.h"

namespace Spyll
{
	constexpr const char* REFLECT_ANNOTATION = "__REFLECT__";

	ReflectionParser::ReflectionParser() {}

	ReflectionParser::~ReflectionParser() {}

	bool
	ReflectionParser::ShouldReflectDecl(const clang::NamedDecl* Decl) const
	{
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

		if (auto ctx = llvm::dyn_cast<clang::CXXRecordDecl>(Decl->getDeclContext()))
		{
			return ShouldReflectDecl(ctx);
		}

		return false;
	}

	bool
	ReflectionParser::ParseCXXRecordDecl(clang::CXXRecordDecl* Decl, clang::SourceManager* a_sourceManager)
	{
		if (!Decl->isCompleteDefinition())
			return true;

		if (Decl->isDependentType())
			return true;

		if (!ShouldReflectDecl(Decl))
			return true;

		m_types.emplace_back(new Type(Decl));

		return true;
	}

	bool
	ReflectionParser::ParseFunctionDecl(clang::FunctionDecl* Decl, clang::SourceManager* a_sourceManager)
	{
		if (Decl->getDeclContext()->isRecord())
			return true;

		if (!Decl->isCanonicalDecl())
			return true;

		if (Decl->getStorageClass() != clang::SC_Extern)
			return true;

		if (!ShouldReflectDecl(Decl))
			return true;

		m_globalFunctions.emplace_back(new Function(Decl, nullptr));

		return true;
	}

	bool
	ReflectionParser::ParseVarDecl(clang::VarDecl* Decl, clang::SourceManager* a_sourceManager)
	{
		if (Decl->getDeclContext()->isRecord())
			return true;

		if (!Decl->isCanonicalDecl())
			return true;

		if (Decl->getStorageClass() != clang::SC_Extern)
			return true;

		if (!ShouldReflectDecl(Decl))
			return true;

		m_globalVariables.emplace_back(new Variable(Decl, nullptr));

		return true;
	}

	bool
	ReflectionParser::ParseEnumDecl(clang::EnumDecl* Decl, clang::SourceManager* a_sourceManager)
	{
		if (!Decl->isCompleteDefinition())
			return true;

		if (!ShouldReflectDecl(Decl))
			return true;

		m_enums.emplace_back(new Enum(Decl));

		return true;
	}
}
