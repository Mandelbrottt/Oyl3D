#include "ReflectionGenerator.h"

#include <clang/AST/RecordLayout.h>

#include "Descriptors/EnumDescriptor.h"
#include "Descriptors/FieldDescriptor.h"
#include "Descriptors/FunctionDescriptor.h"
#include "Descriptors/TypeDescriptor.h"

namespace
{
	constexpr std::string_view REFLECT_ANNOTATION = "reflect";
}

namespace Spyll
{
	struct ReflectionGenerator::Impl
	{
		clang::ASTContext* context;
		
		std::unordered_map<const clang::Decl*, TypeDescriptor> types;
		std::unordered_map<const clang::Decl*, FieldDescriptor> fields;
		std::unordered_map<const clang::Decl*, FunctionDescriptor> functions;
		std::unordered_map<const clang::Decl*, EnumDescriptor> enums;

		DescriptorId nextId = static_cast<DescriptorId>(0);
	};

	ReflectionGenerator::ReflectionGenerator()
	{
		m_impl = new Impl;
	}

	ReflectionGenerator::~ReflectionGenerator()
	{
		delete m_impl;
		m_impl = nullptr;
	}

	bool
	ReflectionGenerator::ShouldReflectDecl(const clang::Decl* Decl) const
	{
		bool shouldBeReflected = false;

		// Reflect if Decl has annotate("reflect") attribute
		for (const auto* iter : Decl->specific_attrs<clang::AnnotateAttr>())
		{
			// __attribute__((annotate("reflect")))
			const clang::AnnotateAttr* attr = clang::dyn_cast<clang::AnnotateAttr>(iter);
			llvm::StringRef annotation = attr->getAnnotation();
			if (annotation.compare(REFLECT_ANNOTATION) == 0)
			{
				shouldBeReflected = true;
				break;
			}
		}

		return shouldBeReflected;
	}

	bool
	ReflectionGenerator::IsScraped(const clang::CXXRecordDecl* Decl) const
	{
		const auto canonicalDecl = Decl->getCanonicalDecl();
		return m_impl->types.find(canonicalDecl) != m_impl->types.end();
	}

	bool
	ReflectionGenerator::IsScraped(const clang::FieldDecl* Decl) const
	{
		const auto canonicalDecl = Decl->getCanonicalDecl();
		return m_impl->fields.find(canonicalDecl) != m_impl->fields.end();
	}

	bool
	ReflectionGenerator::IsScraped(const clang::FunctionDecl* Decl) const
	{
		const auto canonicalDecl = Decl->getCanonicalDecl();
		return m_impl->functions.find(canonicalDecl) != m_impl->functions.end();
	}

	bool
	ReflectionGenerator::IsScraped(const clang::EnumDecl* Decl) const
	{
		const auto canonicalDecl = Decl->getCanonicalDecl();
		return m_impl->enums.find(canonicalDecl) != m_impl->enums.end();
	}

	TypeDescriptor&
	ReflectionGenerator::ScrapeDecl(const clang::CXXRecordDecl* Decl)
	{
		Decl = Decl->getStandardLayoutBaseWithFields();

		if (IsScraped(Decl))
		{
			return m_impl->types[Decl];
		}

		auto& recordLayout = Decl->getASTContext().getASTRecordLayout(Decl);

		TypeDescriptor descriptor;
		descriptor.id = GetNewDescriptorId();
		descriptor.name = Decl->getNameAsString();
		descriptor.isStruct = Decl->isStruct();
		descriptor.size = recordLayout.getSize().getQuantity();

		descriptor.fields.reserve(recordLayout.getFieldCount());
		for (auto iter : Decl->fields())
		{
			auto& field = ScrapeDecl(iter);
			descriptor.fields.emplace_back(field.id);
		}

		// find a way to reserve number of methods
		for (auto iter : Decl->methods())
		{
			auto& function = ScrapeDecl(iter);
			descriptor.functions.emplace_back(function.id);
		}

		auto resultIter = m_impl->types.emplace(Decl, std::move(descriptor)).first;
		auto& result = resultIter->second;
		return result;
	}

	FieldDescriptor&
	ReflectionGenerator::ScrapeDecl(const clang::FieldDecl* Decl)
	{
		Decl = Decl->getCanonicalDecl();

		if (IsScraped(Decl))
		{
			return m_impl->fields.at(Decl);
		}

		FieldDescriptor descriptor;
		
		auto resultIter = m_impl->fields.emplace(Decl, std::move(descriptor)).first;
		auto& result = resultIter->second;
		return result;
	}

	FunctionDescriptor&
	ReflectionGenerator::ScrapeDecl(const clang::FunctionDecl* Decl)
	{
		Decl = Decl->getCanonicalDecl();
		
		if (IsScraped(Decl))
		{
			return m_impl->functions.at(Decl);
		}

		FunctionDescriptor descriptor;
		
		auto resultIter = m_impl->functions.emplace(Decl, std::move(descriptor)).first;
		auto& result = resultIter->second;
		return result;
	}

	EnumDescriptor&
	ReflectionGenerator::ScrapeDecl(const clang::EnumDecl* Decl)
	{
		Decl = Decl->getCanonicalDecl();
		
		if (IsScraped(Decl))
		{
			return m_impl->enums.at(Decl);
		}

		EnumDescriptor descriptor;

		auto resultIter = m_impl->enums.emplace(Decl, std::move(descriptor)).first;
		auto& result = resultIter->second;
		return result;
	}

	void
	ReflectionGenerator::SetContext(clang::ASTContext* Context)
	{
		m_impl->context = Context;
	}

	TypeDescriptor*
	ReflectionGenerator::TryGetType(const clang::CXXRecordDecl* Decl) const
	{
		auto iter = m_impl->types.find(Decl);
		if (iter == m_impl->types.end())
			return nullptr;
		return &iter->second;
	}

	FieldDescriptor*
	ReflectionGenerator::TryGetField(const clang::FieldDecl* Decl) const
	{
		auto iter = m_impl->fields.find(Decl);
		if (iter == m_impl->fields.end())
			return nullptr;
		return &iter->second;
	}

	FunctionDescriptor*
	ReflectionGenerator::TryGetFunction(const clang::FunctionDecl* Decl) const
	{
		auto iter = m_impl->functions.find(Decl);
		if (iter == m_impl->functions.end())
			return nullptr;
		return &iter->second;
	}

	EnumDescriptor*
	ReflectionGenerator::TryGetEnum(const clang::EnumDecl* Decl) const
	{
		auto iter = m_impl->enums.find(Decl);
		if (iter == m_impl->enums.end())
			return nullptr;
		return &iter->second;
	}

	DescriptorId
	ReflectionGenerator::GetNewDescriptorId()
	{
		DescriptorId result = m_impl->nextId;

		auto asUnderlying = static_cast<std::underlying_type_t<DescriptorId>>(result);
		++asUnderlying;
		m_impl->nextId = static_cast<DescriptorId>(asUnderlying);
		
		return result;
	}
}
