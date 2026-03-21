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

		std::unordered_map<std::string, TypeDescriptor> builtIns;
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
		Decl = Decl->getCanonicalDecl();
		return m_impl->types.find(Decl) != m_impl->types.end();
	}

	bool
	ReflectionGenerator::IsScraped(const clang::FieldDecl* Decl) const
	{
		const auto* canonicalDecl = Decl->getCanonicalDecl();
		return m_impl->fields.find(canonicalDecl) != m_impl->fields.end();
	}

	bool
	ReflectionGenerator::IsScraped(const clang::FunctionDecl* Decl) const
	{
		const auto* canonicalDecl = Decl->getCanonicalDecl();
		return m_impl->functions.find(canonicalDecl) != m_impl->functions.end();
	}

	bool
	ReflectionGenerator::IsScraped(const clang::EnumDecl* Decl) const
	{
		const auto* canonicalDecl = Decl->getCanonicalDecl();
		return m_impl->enums.find(canonicalDecl) != m_impl->enums.end();
	}

	TypeDescriptor&
	ReflectionGenerator::ScrapeDecl(const clang::CXXRecordDecl* Decl)
	{
		Decl = Decl->getCanonicalDecl();

		if (IsScraped(Decl))
		{
			return m_impl->types[Decl];
		}

		auto& descriptor = AddType(Decl);

		for (clang::FieldDecl* iter : Decl->fields())
		{
			auto& field = ScrapeDecl(iter);
			descriptor.fields.emplace_back(field.id);
		}

		// find a way to reserve number of methods
		for (clang::FunctionDecl* iter : Decl->methods())
		{
			auto& function = ScrapeDecl(iter);
			descriptor.functions.emplace_back(function.id);
		}

		return descriptor;
	}

	FieldDescriptor&
	ReflectionGenerator::ScrapeDecl(const clang::FieldDecl* Decl)
	{
		Decl = Decl->getCanonicalDecl();

		if (IsScraped(Decl))
		{
			return m_impl->fields.at(Decl);
		}

		clang::QualType qualifiedType = Decl->getType();
		const auto* typePtr = qualifiedType.getTypePtr();
		auto& typeDesc = AddType(typePtr);

		auto parentTypeDecl = llvm::dyn_cast<clang::CXXRecordDecl>(Decl->getParent());

		// Guaranteed to be true, we're inside the parent
		auto& parentTypeDesc = m_impl->types.at(parentTypeDecl->getCanonicalDecl());

		auto& parentRecordLayout = m_impl->context->getASTRecordLayout(parentTypeDecl);
		auto offset = parentRecordLayout.getFieldOffset(Decl->getFieldIndex()) / 8; // Offset is in bits

		FieldDescriptor descriptor;
		descriptor.id = GetNewDescriptorId();
		descriptor.name = Decl->getNameAsString();
		descriptor.type = typeDesc.id;
		descriptor.offset = uint32_t(offset);
		descriptor.owningType = parentTypeDesc.id;

		assert(!Decl->isBitField());

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
		descriptor.id = GetNewDescriptorId();

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
		AddPrimitiveTypes();
	}

	TypeDescriptor&
	ReflectionGenerator::AddType(const clang::CXXRecordDecl* Decl)
	{
		Decl = Decl->getCanonicalDecl();

		if (IsScraped(Decl))
		{
			return m_impl->types.at(Decl);
		}

		auto& descriptor = CreateTypeDescriptor(Decl);
		descriptor.name = Decl->getNameAsString();
		descriptor.isComposite = true;
		descriptor.isStruct = Decl->isStruct();

		auto& recordLayout = Decl->getASTContext().getASTRecordLayout(Decl);
		descriptor.size = uint32_t(recordLayout.getSize().getQuantity());

		return descriptor;
	}

	TypeDescriptor&
	ReflectionGenerator::AddType(const clang::Type* Type)
	{
		Type = Type->getCanonicalTypeInternal().getTypePtr();
		Type = Type->getPointeeOrArrayElementType();

		if (Type->isBuiltinType())
		{
			auto name = Type->getCanonicalTypeInternal().getAsString();
			return m_impl->builtIns.at(name);
		}

		const auto* cxxDecl = Type->getAsCXXRecordDecl()->getCanonicalDecl();
		return ScrapeDecl(cxxDecl);
	}

	TypeDescriptor&
	ReflectionGenerator::CreateTypeDescriptor(const clang::CXXRecordDecl* Decl)
	{
		TypeDescriptor descriptor;
		descriptor.id = GetNewDescriptorId();

		auto resultIter = m_impl->types.emplace(Decl, std::move(descriptor)).first;
		auto& result = resultIter->second;
		return result;
	}

	void
	ReflectionGenerator::AddPrimitiveTypes()
	{
		auto addTypeFn = [&](std::string_view Name, uint32_t Size)
		{
			TypeDescriptor descriptor;
			descriptor.id = GetNewDescriptorId();
			descriptor.name = Name;
			descriptor.isComposite = false;
			descriptor.isStruct = false;
			descriptor.size = Size;

			m_impl->builtIns.emplace(Name, std::move(descriptor));
		};

		addTypeFn("void", 0);
		addTypeFn("bool", sizeof(bool));
		addTypeFn("char", sizeof(char));
		addTypeFn("unsigned char", sizeof(unsigned char));
		addTypeFn("int", sizeof(int));
		addTypeFn("unsigned int", sizeof(unsigned int));
		addTypeFn("float", sizeof(float));
		addTypeFn("double", sizeof(double));
		addTypeFn("wchar_t", sizeof(wchar_t));

		// Clang uses _Bool as the display name for bool, alias it
		m_impl->builtIns["_Bool"] = m_impl->builtIns["bool"];
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
