#pragma once

#include <clang/AST/DeclCXX.h>

#include "Spyll/Spyll/Descriptors/DescriptorEnums.h"
#include "Spyll/Spyll/Descriptors/ReflectionDescriptor.h"

namespace Spyll
{
	namespace Detail
	{
		template<typename T>
		struct DescriptorFromDecl {};
	}

	struct TypeDescriptor;
	struct FieldDescriptor;
	struct VariableDescriptor;
	struct FunctionDescriptor;
	struct EnumDescriptor;

	class ReflectionGenerator
	{
	public:
		ReflectionGenerator();
		~ReflectionGenerator();

		bool
		ShouldReflectDecl(const clang::NamedDecl* Decl) const;

		bool
		IsScraped(const clang::CXXRecordDecl* Decl) const;

		// Non-static-member variables
		bool
		IsScraped(const clang::FieldDecl* Decl) const;

		// global and static-member variables, function parameters
		bool
		IsScraped(const clang::VarDecl* Decl) const;

		// Static and Non-static
		bool
		IsScraped(const clang::FunctionDecl* Decl) const;

		bool
		IsScraped(const clang::EnumDecl* Decl) const;

		TypeDescriptor&
		ScrapeDecl(const clang::CXXRecordDecl* Decl);

		FieldDescriptor&
		ScrapeDecl(const clang::FieldDecl* Decl);

		VariableDescriptor&
		ScrapeDecl(const clang::VarDecl* Decl);

		FunctionDescriptor&
		ScrapeDecl(const clang::FunctionDecl* Decl);

		EnumDescriptor&
		ScrapeDecl(const clang::EnumDecl* Decl);

		void
		Setup(clang::ASTContext* Context);

		ReflectionDescriptor
		GetReflectionDescriptor() const;

	private:
		TypeDescriptor&
		AddType(const clang::CXXRecordDecl* Decl);

		TypeDescriptor&
		AddType(const clang::Type* Type);

		TypeDescriptor&
		GetOrAddTypeDescriptor(const void* Decl);

		void
		AddPrimitiveTypes();

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
