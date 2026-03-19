#include "pch.h"
#include "ReflectionGenerator.h"

namespace
{
	constexpr std::string_view REFLECT_ANNOTATION = "reflect";
}

namespace Spyll
{
	void
	ReflectionGenerator::ScrapeDecl(clang::ASTContext* Context, clang::Decl* Decl)
	{
		(void) Context;
		
		if (!Decl || !ShouldReflectDecl(Decl))
		{
			return;
		}

		if (auto cxxRecordDecl = clang::dyn_cast<clang::CXXRecordDecl>(Decl))
		{
			ScrapeCxxRecordDecl(cxxRecordDecl);
			return;
		}

		if (auto enumDecl = clang::dyn_cast<clang::EnumDecl>(Decl))
		{
			ScrapeEnumDecl(enumDecl);
			return;
		}

		if (auto functionDecl = clang::dyn_cast<clang::FunctionDecl>(Decl))
		{
			ScrapeFunctionDecl(functionDecl);
			return;
		}
	}

	bool
	ReflectionGenerator::ShouldReflectDecl(clang::Decl* Decl) const
	{
		if (!Decl)
		{
			return false;
		}

		bool shouldBeReflected = false;

		// Reflect if Decl has annotate("reflect") attribute
		for (const auto* iter : Decl->specific_attrs<clang::AnnotateAttr>())
		{
			// __attribute__((annotate("reflect")))
			const clang::AnnotateAttr* attr = clang::dyn_cast<clang::AnnotateAttr>(iter);
			llvm::StringRef annotation = attr->getAnnotation();
			if (annotation.compare(REFLECT_ANNOTATION))
			{
				shouldBeReflected = true;
				break;
			}
		}
		
		return shouldBeReflected;
	}

	void
	ReflectionGenerator::ScrapeCxxRecordDecl(clang::CXXRecordDecl* Decl)
	{
		(void) Decl;
	}

	void
	ReflectionGenerator::ScrapeEnumDecl(clang::EnumDecl* Decl)
	{
		(void) Decl;
	}

	void
	ReflectionGenerator::ScrapeFunctionDecl(clang::FunctionDecl* Decl)
	{
		(void) Decl;
	}
}
