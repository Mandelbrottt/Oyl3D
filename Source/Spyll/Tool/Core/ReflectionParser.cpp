#include "ReflectionParser.h"

#include "Spyll/Tool/Core/Declarations/Class.h"

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
	ReflectionParser::ParseRecordDecl(clang::RecordDecl* Decl)
	{
		if (!Decl->isCompleteDefinition())
			return true;

		// Don't parse unspecialized template types
		if (Decl->isDependentType())
			return true;

		if (Decl->getKind() != clang::Decl::Record)
			return true;

		if (!ShouldReflectDecl(Decl))
			return true;

		m_records.emplace_back(Decl);

		return true;
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

		m_classes.emplace_back(Decl);

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

		m_enums.emplace_back(Decl);

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
		for (size_t i = m_primitives.size() - 1; i != 0; i--)
		{
			auto& type = m_primitives[i];
			if (type.GetClangDecl() == pParentDecl)
				return &type;
		}

		return nullptr;
	}

	void
	ReflectionParser::PopulateTypeFields()
	{
		auto addToTypesList = [&](auto& a_list)
		{
			for (auto& type : a_list)
			{
				m_types.emplace_back(&type);
				m_typeIndexMap[type.GetClangDecl()] = m_types.size() - 1;
			}
		};

		addToTypesList(m_primitives);
		addToTypesList(m_enums);
		addToTypesList(m_records);
		addToTypesList(m_classes);

		auto getSpyllTypeFromClangType = [&](const clang::QualType& a_clangType) -> Type* {
			const auto* typeDecl = a_clangType->getAsRecordDecl();
			auto iter = m_typeIndexMap.find(typeDecl);
			if (iter == m_typeIndexMap.end())
				return nullptr;
			return m_types[iter->second];
		};

		for (auto& type : m_types)
		{
			const auto* typeDecl = type->GetClangDecl();

			if (typeDecl->getKind() < clang::Decl::firstRecord || typeDecl->getKind() > clang::Decl::lastRecord)
				continue;
			auto& record = *static_cast<Record*>(type);

			for (auto& field : record.m_fields)
			{
				field.m_type = getSpyllTypeFromClangType(field.GetClangDecl()->getType());
			}

			if (typeDecl->getKind() < clang::Decl::firstCXXRecord || typeDecl->getKind() > clang::Decl::lastCXXRecord)
				continue;
			auto& class_ = *static_cast<Class*>(type);

			for (const auto& base : class_.GetClangDecl()->bases())
			{
				auto* baseDecl = base.getType()->getAsRecordDecl();
				auto baseIndex = m_typeIndexMap[baseDecl];
				auto* baseType = dynamic_cast<Record*>(m_types[baseIndex]);
				class_.m_bases.emplace_back(baseType, base.isVirtual());
			}

			for (auto& variable : class_.m_variables)
			{
				variable.m_type = getSpyllTypeFromClangType(variable.GetClangDecl()->getType());
			}

			for (auto& method : class_.m_methods)
			{
				method.m_returnType = getSpyllTypeFromClangType(method.GetClangDecl()->getReturnType());
				for (auto& argument : method.m_arguments)
					argument.m_type = getSpyllTypeFromClangType(argument.GetClangDecl()->getType());
			}

			for (auto& function : class_.m_functions)
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
