#pragma once

#include <clang/AST/DeclCXX.h>

#include "Spyll/Spyll/Descriptors/DescriptorEnums.h"

namespace Spyll
{
	namespace Detail
	{
		template<typename T>
		struct DescriptorFromDecl {};
	}

	struct TypeDescriptor;
	struct FieldDescriptor;
	struct FunctionDescriptor;
	struct EnumDescriptor;

	class ReflectionGenerator
	{
	public:
		ReflectionGenerator();
		~ReflectionGenerator();

		bool
		ShouldReflectDecl(const clang::Decl* Decl) const;

		bool
		IsScraped(const clang::CXXRecordDecl* Decl) const;

		bool
		IsScraped(const clang::FieldDecl* Decl) const;

		bool
		IsScraped(const clang::FunctionDecl* Decl) const;

		bool
		IsScraped(const clang::EnumDecl* Decl) const;

		TypeDescriptor&
		ScrapeDecl(const clang::CXXRecordDecl* Decl);

		FieldDescriptor&
		ScrapeDecl(const clang::FieldDecl* Decl);

		FunctionDescriptor&
		ScrapeDecl(const clang::FunctionDecl* Decl);

		EnumDescriptor&
		ScrapeDecl(const clang::EnumDecl* Decl);

		void
		SetContext(clang::ASTContext* Context);

	private:
		TypeDescriptor&
		AddType(const clang::CXXRecordDecl* Decl);

		TypeDescriptor&
		AddType(const clang::Type* Type);

		TypeDescriptor&
		CreateTypeDescriptor(const clang::CXXRecordDecl* Decl);

		void
		AddPrimitiveTypes();

		DescriptorId
		GetNewDescriptorId();

	private:
		struct Impl;
		Impl* m_impl;
	};

	namespace Detail
	{
		template<>
		struct DescriptorFromDecl<clang::CXXRecordDecl>
		{
			using type = TypeDescriptor;
		};

		template<>
		struct DescriptorFromDecl<clang::FieldDecl>
		{
			using type = FieldDescriptor;
		};

		template<>
		struct DescriptorFromDecl<clang::FunctionDecl>
		{
			using type = FunctionDescriptor;
		};

		template<>
		struct DescriptorFromDecl<clang::EnumDecl>
		{
			using type = EnumDescriptor;
		};
	}
}
