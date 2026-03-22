#include "ReflectionGenerator.h"

#include <clang/AST/RecordLayout.h>

#include "Spyll/Spyll/Descriptors/EnumDescriptor.h"
#include "Spyll/Spyll/Descriptors/FieldDescriptor.h"
#include "Spyll/Spyll/Descriptors/FunctionDescriptor.h"
#include "Spyll/Spyll/Descriptors/TypeDescriptor.h"

namespace
{
	constexpr std::string_view REFLECT_ANNOTATION = "reflect";
}

namespace Spyll
{
	struct ReflectionGenerator::Impl
	{
		clang::ASTContext* context;

		std::unordered_map<const void*, TypeDescriptor> types;
		std::unordered_map<const void*, FieldDescriptor> fields;
		std::unordered_map<const void*, FunctionDescriptor> functions;
		std::unordered_map<const void*, EnumDescriptor> enums;

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

		//Decl->getASTContext()

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
		auto offset = parentRecordLayout.getFieldOffset(Decl->getFieldIndex()); // Offset is in bits

		FieldDescriptor descriptor;
		descriptor.id = GetNewDescriptorId();
		descriptor.name = Decl->getNameAsString();
		descriptor.type = typeDesc.id;
		descriptor.offsetInBits = uint32_t(offset);
		descriptor.owningType = parentTypeDesc.id;

		// FieldDecl is by definition non-static, VarDecl can be static
		descriptor.isStatic = false;

		auto cvrQualifiers = qualifiedType.getCVRQualifiers();
		descriptor.isConst = cvrQualifiers & clang::Qualifiers::Const;
		descriptor.isVolatile = cvrQualifiers & clang::Qualifiers::Volatile;

		descriptor.isPointer = qualifiedType->isPointerType();
		descriptor.isReference = qualifiedType->isLValueReferenceType();

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

		auto& descriptor = GetOrAddTypeDescriptor(Decl);
		descriptor.name = Decl->getNameAsString();
		descriptor.isComposite = true;
		descriptor.isStruct = Decl->isStruct();

		auto& recordLayout = Decl->getASTContext().getASTRecordLayout(Decl);
		descriptor.sizeInBits = uint32_t(recordLayout.getSize().getQuantity()) * 8;

		return descriptor;
	}

	TypeDescriptor&
	ReflectionGenerator::AddType(const clang::Type* Type)
	{
		if (Type->isArrayType())
			Type = Type->getBaseElementTypeUnsafe();

		if (Type->isPointerType() || Type->isLValueReferenceType())
			Type = Type->getPointeeType().getTypePtr();

		Type = Type->getCanonicalTypeInternal().getTypePtr();

		if (auto iter = m_impl->types.find(Type); iter != m_impl->types.end())
		{
			return iter->second;
		}

		if (const auto* cxxDecl = Type->getAsCXXRecordDecl())
		{
			cxxDecl = cxxDecl->getCanonicalDecl();
			return ScrapeDecl(cxxDecl);
		}

		// Unknown type
		auto& descriptor = GetOrAddTypeDescriptor(Type);
		descriptor.name = Type->getCanonicalTypeInternal().getAsString();
		descriptor.isComposite = Type->isCompoundType();
		descriptor.isStruct = Type->isStructureType();
		descriptor.sizeInBits = m_impl->context->getTypeSize(Type);
		return descriptor;
	}

	TypeDescriptor&
	ReflectionGenerator::GetOrAddTypeDescriptor(const void* Decl)
	{
		if (auto iter = m_impl->types.find(Decl); iter != m_impl->types.end())
		{
			return iter->second;
		}

		TypeDescriptor descriptor;
		descriptor.id = GetNewDescriptorId();

		auto resultIter = m_impl->types.emplace(Decl, std::move(descriptor)).first;
		auto& result = resultIter->second;
		return result;
	}

	void
	ReflectionGenerator::AddPrimitiveTypes()
	{
		AddType(m_impl->context->VoidTy.getTypePtr());
		AddType(m_impl->context->BoolTy.getTypePtr());
		AddType(m_impl->context->CharTy.getTypePtr());
		AddType(m_impl->context->ShortTy.getTypePtr());
		AddType(m_impl->context->IntTy.getTypePtr());
		AddType(m_impl->context->LongTy.getTypePtr());
		AddType(m_impl->context->UnsignedCharTy.getTypePtr());
		AddType(m_impl->context->UnsignedShortTy.getTypePtr());
		AddType(m_impl->context->UnsignedIntTy.getTypePtr());
		AddType(m_impl->context->UnsignedLongTy.getTypePtr());
		AddType(m_impl->context->FloatTy.getTypePtr());
		AddType(m_impl->context->DoubleTy.getTypePtr());
		AddType(m_impl->context->WCharTy.getTypePtr());
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
