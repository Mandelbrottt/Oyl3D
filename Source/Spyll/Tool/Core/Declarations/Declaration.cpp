#include "Declaration.h"

#include <filesystem>

#include <clang/AST/Decl.h>

#include "Type.h"

namespace Spyll
{
	inline
	AccessSpecifier
	FromClang(clang::AccessSpecifier a_spec)
	{
		AccessSpecifier spyllSpecs[(size_t) AccessSpecifier::Count];
		spyllSpecs[clang::AS_public] = AccessSpecifier::Public;
		spyllSpecs[clang::AS_protected] = AccessSpecifier::Protected;
		spyllSpecs[clang::AS_private] = AccessSpecifier::Private;
		spyllSpecs[clang::AS_none] = AccessSpecifier::None;
		return spyllSpecs[a_spec];
	}

	Declaration::Declaration(const clang::NamedDecl* a_decl)
		: m_enabled(true),
		  m_qualifiedName(a_decl->getQualifiedNameAsString()),
		  m_attributeParser(a_decl),
		  m_decl(a_decl)
	{
		llvm::raw_string_ostream out { m_name };
		a_decl->getDeclName().print(out, a_decl->getASTContext().getPrintingPolicy());

		auto sourceLocation = a_decl->getLocation();

		m_sourceFile = a_decl->getASTContext().getSourceManager().getFilename(sourceLocation);
		m_sourceFile = absolute(std::filesystem::path(m_sourceFile).lexically_normal()).string();

		m_sourceLine = a_decl->getASTContext().getSourceManager().getSpellingLineNumber(sourceLocation);
	}

	bool
	Declaration::ShouldReflect() const
	{
		return m_enabled;
	}

	AccessSpecifier
	Declaration::GetAccessSpecifier() const
	{
		return FromClang(m_decl->getAccess());
	}

	std::string_view
	Declaration::GetSourceFile() const
	{
		return m_sourceFile;
	}

	std::uint32_t
	Declaration::GetSourceLine() const
	{
		return m_sourceLine;
	}

	const clang::NamedDecl*
	Declaration::GetClangDecl() const
	{
		return m_decl;
	}

	std::string
	Declaration::ToString() const
	{
		std::stringstream stream;

		// Attributes
		stream << "[[";
		const auto& attributes = GetAttributes();
		for (size_t i = 0; i < attributes.size(); i++)
		{
			const auto& attribute = attributes[i];
			stream << attribute.ToString();
			if (i != attributes.size() - 1)
			{
				stream << ", ";
			}
		}
		stream << "]]";

		// Name and location
		stream << " " << m_qualifiedName;
		stream << " (" << m_sourceFile << ":" << m_sourceLine;

		return std::move(stream.str());
	}

	static
	bool
	AreTemplateParamsVisible(const clang::CXXRecordDecl* a_decl)
	{
		assert(a_decl);

		auto templateSpecializationDecl = clang::dyn_cast<clang::ClassTemplateSpecializationDecl>(a_decl);
		if (!templateSpecializationDecl)
			return true;

		for (const auto& arg : templateSpecializationDecl->getTemplateArgs().asArray())
		{
			if (arg.getKind() != clang::TemplateArgument::Type)
				continue;

			auto argDecl = arg.getAsType()->getAsRecordDecl();
			if (!argDecl)
				continue;

			bool isVisible = argDecl->getAccess() != clang::AS_protected
			                 && argDecl->getAccess() != clang::AS_private;

			if (!isVisible)
				return false;

			auto cxxRecordDecl = clang::dyn_cast<clang::CXXRecordDecl>(argDecl);
			if (cxxRecordDecl && !AreTemplateParamsVisible(cxxRecordDecl))
				return false;
		}

		return true;
	}

	bool
	IsTypeOfDeclVisible(const clang::ValueDecl* a_decl)
	{
		auto type = a_decl->getType().getTypePtr();
		while (type->isPointerType())
			type = type->getPointeeOrArrayElementType();

		auto recordDecl = type->getAsRecordDecl();
		if (!recordDecl)
			return true;

		bool isTypeVisible = recordDecl->getAccess() != clang::AS_protected
		                     && recordDecl->getAccess() != clang::AS_private;
		if (!isTypeVisible)
			return false;

		// Recursively check visibility of all template parameters
		auto cxxRecordDecl = clang::dyn_cast<clang::CXXRecordDecl>(recordDecl);
		if (cxxRecordDecl && !AreTemplateParamsVisible(cxxRecordDecl))
			return false;

		return true;
	}
}
