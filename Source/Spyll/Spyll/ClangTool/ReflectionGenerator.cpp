#include "ReflectionGenerator.h"

#include <clang/AST/RecordLayout.h>

#include "Spyll/Spyll/Descriptors/EnumDescriptor.h"
#include "Spyll/Spyll/Descriptors/FieldDescriptor.h"
#include "Spyll/Spyll/Descriptors/FunctionDescriptor.h"
#include "Spyll/Spyll/Descriptors/TypeDescriptor.h"
#include "Spyll/Spyll/Descriptors/VariableDescriptor.h"

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
		std::unordered_map<const void*, VariableDescriptor> variables;
		std::unordered_map<const void*, FunctionDescriptor> functions;
		std::unordered_map<const void*, EnumDescriptor> enums;
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
	ReflectionGenerator::ShouldReflectDecl(const clang::NamedDecl* Decl) const
	{
		// Reflect if Decl has annotate("reflect") attribute
		for (const auto* iter : Decl->specific_attrs<clang::AnnotateAttr>())
		{
			// __attribute__((annotate("reflect")))
			const clang::AnnotateAttr* attr = clang::dyn_cast<clang::AnnotateAttr>(iter);
			llvm::StringRef annotation = attr->getAnnotation();
			if (annotation.compare(REFLECT_ANNOTATION) == 0)
			{
				return true;
			}
		}

		if (auto context = llvm::dyn_cast<clang::RecordDecl>(Decl->getDeclContext()))
		{
			return ShouldReflectDecl(context);
		}

		return false;
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
	ReflectionGenerator::IsScraped(const clang::VarDecl* Decl) const
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

		if (!ShouldReflectDecl(Decl))
		{
			// Return an effectively opaque type with minimal information
			return descriptor;
		}

		descriptor.isOpaque = false;

		for (clang::FieldDecl* iter : Decl->fields())
		{
			auto& field = ScrapeDecl(iter);
			field.ownerType = descriptor.id;
		}

		for (clang::FunctionDecl* iter : Decl->methods())
		{
			auto& function = ScrapeDecl(iter);
			function.ownerType = descriptor.id;
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
		const auto& typeDesc = AddType(typePtr);

		const auto* parentTypeDecl = llvm::dyn_cast<clang::CXXRecordDecl>(Decl->getParent());

		auto& parentRecordLayout = m_impl->context->getASTRecordLayout(parentTypeDecl);
		auto offset = parentRecordLayout.getFieldOffset(Decl->getFieldIndex()); // Offset is in bits

		FieldDescriptor descriptor;
		descriptor.id = GetNewDescriptorId<FieldDescriptorId>();
		descriptor.name = Decl->getName();
		descriptor.type = typeDesc.id;
		descriptor.offsetInBits = uint32_t(offset);

		auto cvrQualifiers = qualifiedType.getCVRQualifiers();
		descriptor.isConst = cvrQualifiers & clang::Qualifiers::Const;
		descriptor.isVolatile = cvrQualifiers & clang::Qualifiers::Volatile;

		descriptor.isPointer = qualifiedType->isPointerType();
		descriptor.isReference = qualifiedType->isLValueReferenceType();

		descriptor.accessSpecifier = ToAccessSpecifier(Decl->getAccess());

		auto resultIter = m_impl->fields.emplace(Decl, std::move(descriptor)).first;
		auto& result = resultIter->second;
		return result;
	}

	VariableDescriptor&
	ReflectionGenerator::ScrapeDecl(const clang::VarDecl* Decl)
	{
		Decl = Decl->getCanonicalDecl();

		if (IsScraped(Decl))
		{
			return m_impl->variables.at(Decl);
		}

		clang::QualType qualifiedType = Decl->getType();
		const auto* typePtr = qualifiedType.getTypePtr();
		const auto& typeDesc = AddType(typePtr);

		// Guaranteed to be true, we're inside the parent
		VariableDescriptor descriptor;
		descriptor.id = GetNewDescriptorId<VariableDescriptorId>();
		descriptor.type = typeDesc.id;

		// Special check for static members
		if (const auto* parentDecl = llvm::dyn_cast<clang::CXXRecordDecl>(Decl->getDeclContext()))
		{
			descriptor.accessSpecifier = ToAccessSpecifier(parentDecl->getAccess());

			// Add this field to parent descriptor, static fields aren't already iterated
			// parent is likely already scraped, just easy to get descriptor this way
			auto& parentDesc = ScrapeDecl(parentDecl);
			descriptor.ownerType = parentDesc.id;

			// When decl is a member, the qualified name is implicit
			descriptor.name = Decl->getName();
		} else
		{
			descriptor.name = Decl->getQualifiedNameAsString();
		}

		auto cvrQualifiers = qualifiedType.getCVRQualifiers();
		descriptor.isConst = cvrQualifiers & clang::Qualifiers::Const;
		descriptor.isVolatile = cvrQualifiers & clang::Qualifiers::Volatile;

		descriptor.isPointer = qualifiedType->isPointerType();
		descriptor.isReference = qualifiedType->isLValueReferenceType();

		auto resultIter = m_impl->variables.emplace(Decl, std::move(descriptor)).first;
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
		descriptor.id = GetNewDescriptorId<FunctionDescriptorId>();

		const auto* returnType = Decl->getReturnType().getTypePtr();
		const auto& returnDesc = AddType(returnType);

		descriptor.returnType = returnDesc.id;

		const auto cvQualifiers = Decl->getReturnType().getCVRQualifiers();
		descriptor.isReturnConst = cvQualifiers & clang::Qualifiers::Const;
		descriptor.isReturnVolatile = cvQualifiers & clang::Qualifiers::Volatile;

		descriptor.isReturnPointer = returnType->isPointerType();
		descriptor.isReturnReference = returnType->isLValueReferenceType();

		// Member function specific logic
		if (const auto* methodDecl = llvm::dyn_cast<clang::CXXMethodDecl>(Decl))
		{
			// Qualified name is implicit for members
			descriptor.name = methodDecl->getName();

			descriptor.accessSpecifier = ToAccessSpecifier(methodDecl->getAccess());
			descriptor.isStatic = methodDecl->isStatic();
			descriptor.isConst = methodDecl->isConst();
			descriptor.isVolatile = methodDecl->isVolatile();

			descriptor.isVirtual = methodDecl->isVirtual();
			descriptor.isPureVirtual = methodDecl->isPureVirtual();
			descriptor.isOverride = methodDecl->size_overridden_methods() != 0;
		} else
		{
			descriptor.name = Decl->getQualifiedNameAsString();

			descriptor.accessSpecifier = AccessSpecifier::None;
			descriptor.isStatic = false;
			descriptor.isConst = false;
			descriptor.isVolatile = false;

			descriptor.isVirtual = false;
			descriptor.isPureVirtual = false;
			descriptor.isOverride = false;
		}

		for (const auto* paramDecl : Decl->parameters())
		{
			auto& paramDescriptor = ScrapeDecl(paramDecl);
			paramDescriptor.ownerFunction = descriptor.id;
		}

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
		descriptor.id = GetNewDescriptorId<EnumDescriptorId>();
		descriptor.name = Decl->getQualifiedNameAsString();

		const auto& underlyingTypeDesc = AddType(Decl->getIntegerType().getTypePtr());
		descriptor.underlyingType = underlyingTypeDesc.id;

		size_t numEnumerations = range_size(Decl->enumerators());
		descriptor.enumerations.reserve(numEnumerations);
		for (auto iter : Decl->enumerators())
		{
			descriptor.enumerations.emplace_back(
				iter->getName(), 
				iter->getValue().getLimitedValue()
			);
		}

		auto resultIter = m_impl->enums.emplace(Decl, std::move(descriptor)).first;
		auto& result = resultIter->second;
		return result;
	}

	void
	ReflectionGenerator::Setup(clang::ASTContext* Context)
	{
		m_impl->context = Context;

		// Print names without type class and with namespaces when getting as string
		auto policy = m_impl->context->getPrintingPolicy();
		policy.FullyQualifiedName = true;
		m_impl->context->setPrintingPolicy(policy);
		
		AddPrimitiveTypes();

		ResetDescriptorId<TypeDescriptorId>();
		ResetDescriptorId<FieldDescriptorId>();
		ResetDescriptorId<FunctionDescriptorId>();
		ResetDescriptorId<VariableDescriptorId>();
		ResetDescriptorId<EnumDescriptorId>();
		ResetDescriptorId<TypeDescriptorId>();
	}

	TranslationUnitDescriptor
	ReflectionGenerator::GetTranslationUnitDescriptor() const
	{
		TranslationUnitDescriptor descriptor;

		auto addToDescriptorFn = [](auto& descriptorMap, auto& implMap)
		{
			descriptorMap.reserve(implMap.size());
			for (const auto& [_, desc] : implMap)
			{
				descriptorMap.emplace(desc.id, &desc);
			}
		};

		addToDescriptorFn(descriptor.types,     m_impl->types);
		addToDescriptorFn(descriptor.fields,    m_impl->fields);
		addToDescriptorFn(descriptor.functions, m_impl->functions);
		addToDescriptorFn(descriptor.variables, m_impl->variables);
		addToDescriptorFn(descriptor.enums,     m_impl->enums);

		return descriptor;
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
		descriptor.name = Decl->getTypeForDecl()
							  ->getCanonicalTypeInternal()
							  .getAsString(m_impl->context->getPrintingPolicy());

		descriptor.isComposite = true;
		descriptor.isStruct = Decl->isStruct();

		auto& recordLayout = Decl->getASTContext().getASTRecordLayout(Decl);
		descriptor.sizeInBits = uint32_t(recordLayout.getSize().getQuantity()) * 8;
		descriptor.alignment = uint8_t(recordLayout.getPreferredAlignment().getQuantity());

		descriptor.isOpaque = true;

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
			return ScrapeDecl(cxxDecl);
		}

		// Unknown type
		auto& descriptor = GetOrAddTypeDescriptor(Type);
		descriptor.name = Type->getCanonicalTypeInternal()
							  .getAsString(m_impl->context->getPrintingPolicy());
		descriptor.isComposite = Type->isCompoundType();
		descriptor.isStruct = Type->isStructureType();
		descriptor.sizeInBits = uint32_t(m_impl->context->getTypeSize(Type));
		descriptor.alignment = uint8_t(m_impl->context->getPreferredTypeAlign(Type));
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
		descriptor.id = GetNewDescriptorId<TypeDescriptorId>();

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
}
