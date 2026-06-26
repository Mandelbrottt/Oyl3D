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
	ReflectionParser::ParseStaticAssertDecl(clang::StaticAssertDecl* Decl)
	{
		// If there's no message, then this isn't an annotation decl
		auto messageExpr = Decl->getMessage();
		if (!messageExpr)
			return true;

		// Get the condition and the message as string literals
		auto exprLiteral = clang::dyn_cast<clang::StringLiteral>(Decl->getAssertExpr()->IgnoreImplicitAsWritten());
		auto messageLiteral = clang::dyn_cast<clang::StringLiteral>(messageExpr);

		if (!exprLiteral || !messageLiteral)
		{
			return true;
		}

		// static_assert("__ATTRIBUTE__", "Attribute")
		if (exprLiteral->getString().compare("__ATTRIBUTE__") != 0)
		{
			return true;
		}

		// Iterate the "next" decl until we get to a named declaration that we actually care about
		// ONLY skip static_assert, assume that multiple in a row are all attached to the same declaration
		clang::Decl* nextDecl = Decl;
		do
		{
			nextDecl = nextDecl->getNextDeclInContext();
		} while (nextDecl && nextDecl->getKind() == clang::Decl::StaticAssert);

		// Not attached to a NamedDecl, skip
		if (!nextDecl)
		{
			return true;
		}

		// Found a decl we want to attach the annotation to. Create an annotation attribute and add to the named decl
		// This is guaranteed to exist when we visit the NamedDecl since Visit* runs in order of declaration
		if (auto* namedDecl = clang::dyn_cast<clang::NamedDecl>(nextDecl))
		{
			// Implicit annotation attribute using the same source range as the static_assert
			auto info = clang::AttributeCommonInfo(
				Decl->getSourceRange(),
				clang::AttributeCommonInfo::Kind::AT_Annotate,
				clang::AttributeCommonInfo::Form::Implicit()
			);

			auto* newAttr = clang::AnnotateAttr::Create(Decl->getASTContext(), messageLiteral->getString(), info);
			namedDecl->addAttr(newAttr);
		}

		return true;
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

		m_types.emplace_back(new Type(Decl, a_sourceManager));

		return true;
	}

	bool
	ReflectionParser::ParseFunctionDecl(clang::FunctionDecl* Decl, clang::SourceManager* a_sourceManager)
	{
		if (Decl->getDeclContext()->isRecord())
			return true;

		if (!Decl->isCanonicalDecl())
			return true;

		if (!Decl->getStorageClass() == clang::SC_Extern)
			return true;

		if (!ShouldReflectDecl(Decl))
			return true;

		m_globalFunctions.emplace_back(new Function(Decl, nullptr, a_sourceManager));

		return true;
	}

	bool
	ReflectionParser::ParseVarDecl(clang::VarDecl* Decl, clang::SourceManager* a_sourceManager)
	{
		if (Decl->getDeclContext()->isRecord())
			return true;

		if (!Decl->isCanonicalDecl())
			return true;

		if (!Decl->getStorageClass() == clang::SC_Extern)
			return true;

		if (!ShouldReflectDecl(Decl))
			return true;

		m_globalVariables.emplace_back(new Variable(Decl, nullptr, a_sourceManager));

		return true;
	}

	bool
	ReflectionParser::ParseEnumDecl(clang::EnumDecl* Decl, clang::SourceManager* a_sourceManager)
	{
		if (!Decl->isCompleteDefinition())
			return true;

		if (!ShouldReflectDecl(Decl))
			return true;

		m_enums.emplace_back(new Enum(Decl, a_sourceManager));

		return true;
	}
}
