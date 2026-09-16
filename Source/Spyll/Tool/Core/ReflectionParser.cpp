#include "ReflectionParser.h"

namespace Spyll
{
	constexpr const char* REFLECT_ANNOTATION = "__REFLECT__";

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
	ReflectionParser::ParseCXXRecordDecl(clang::CXXRecordDecl* Decl)
	{
		if (!Decl->isCompleteDefinition())
			return true;

		// Don't parse unspecialized template types
		if (Decl->isDependentType())
			return true;

		if (!ShouldReflectDecl(Decl))
			return true;

		// If this type is inside another, search for the parent
		Type* pParentType = TryGetParentTypeOfDecl(Decl);
		m_types.emplace_back(Decl, pParentType);

		// We just added the type, so we know it's the last element
		m_typeIndexMap[Decl] = m_types.size() - 1;

		return true;
	}

	bool
	ReflectionParser::ParseGlobalFunctionDecl(clang::FunctionDecl* Decl)
	{
		if (Decl->getDeclContext()->isRecord())
			return true;

		if (!Decl->isCanonicalDecl())
			return true;

		if (Decl->getStorageClass() != clang::SC_Extern)
			return true;

		if (!ShouldReflectDecl(Decl))
			return true;

		m_globalFunctions.emplace_back(Decl);

		return true;
	}

	bool
	ReflectionParser::ParseGlobalVarDecl(clang::VarDecl* Decl)
	{
		if (Decl->getDeclContext()->isRecord())
			return true;

		if (!Decl->isCanonicalDecl())
			return true;

		if (Decl->getStorageClass() != clang::SC_Extern)
			return true;

		if (!ShouldReflectDecl(Decl))
			return true;

		m_globalVariables.emplace_back(Decl);

		return true;
	}

	bool
	ReflectionParser::ParseEnumDecl(clang::EnumDecl* Decl)
	{
		if (!Decl->isCompleteDefinition())
			return true;

		if (!ShouldReflectDecl(Decl))
			return true;

		Type* pParentType = TryGetParentTypeOfDecl(Decl);
		m_enums.emplace_back(Decl, pParentType);

		return true;
	}

	Type*
	ReflectionParser::TryGetParentTypeOfDecl(const clang::NamedDecl* Decl)
	{
		// Only search if parent is a record
		auto* pContext = Decl->getNonTransparentDeclContext();
		if (!pContext->isRecord())
			return nullptr;

		// Reverse search the array for the parent type. Clang does pre-order traversal
		// Children are always added after their parent and before the next top-level type
		auto* pParentDecl = clang::dyn_cast<clang::CXXRecordDecl>(pContext);
		for (size_t i = m_types.size() - 1; i != 0; i--)
		{
			auto& type = m_types[i];
			if (type.GetClangDecl() == pParentDecl)
				return &type;
		}

		return nullptr;
	}

	void
	ReflectionParser::PopulateTypeFields()
	{
		auto getSpyllTypeFromClangType = [&](const clang::QualType& a_clangType) -> Type* {
			if (!a_clangType->isRecordType())
				return nullptr;

			const auto* typeDecl = a_clangType->getAsRecordDecl();
			auto typeIndex = m_typeIndexMap[typeDecl];
			return &m_types[typeIndex];
		};

		for (auto& type : m_types)
		{
			const auto* typeDecl = type.GetClangDecl();
			for (const auto& base : typeDecl->bases())
			{
				auto* baseDecl = base.getType()->getAsRecordDecl();
				auto baseIndex = m_typeIndexMap[baseDecl];
				auto& baseType = m_types[baseIndex];
				type.m_baseTypes.emplace_back(&baseType, base.isVirtual());
			}

			for (auto& field : type.m_fields)
			{
				field.m_type = getSpyllTypeFromClangType(field.GetClangDecl()->getType());
			}

			for (auto& variable : type.m_variables)
			{
				variable.m_type = getSpyllTypeFromClangType(variable.GetClangDecl()->getType());
			}

			for (auto& method : type.m_methods)
			{
				method.m_returnType = getSpyllTypeFromClangType(method.GetClangDecl()->getReturnType());
				for (auto& argument : method.m_arguments)
					argument.m_type = getSpyllTypeFromClangType(argument.GetClangDecl()->getType());
			}

			for (auto& function : type.m_functions)
			{
				function.m_returnType = getSpyllTypeFromClangType(function.GetClangDecl()->getReturnType());
				for (auto& argument : function.m_arguments)
					argument.m_type = getSpyllTypeFromClangType(argument.GetClangDecl()->getType());
			}
		}

		for (auto& variable : m_globalVariables)
		{
			variable.m_type = getSpyllTypeFromClangType(variable.GetClangDecl()->getType());
		}

		for (auto& function : m_globalFunctions)
		{
			function.m_returnType = getSpyllTypeFromClangType(function.GetClangDecl()->getReturnType());
			for (auto& argument : function.m_arguments)
				argument.m_type = getSpyllTypeFromClangType(argument.GetClangDecl()->getType());
		}

		for (auto& enum_ : m_enums)
		{
			enum_.m_underlyingType = getSpyllTypeFromClangType(enum_.GetClangDecl()->getIntegerType());
		}
	}
}
