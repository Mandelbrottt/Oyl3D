#pragma once

#include "Descriptors/EnumDescriptor.h"
#include "Descriptors/FieldDescriptor.h"
#include "Descriptors/FunctionDescriptor.h"
#include "Descriptors/TypeDescriptor.h"

namespace Spyll
{
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
		TypeDescriptor*
		TryGetType(const clang::CXXRecordDecl* Decl) const;

		FieldDescriptor*
		TryGetField(const clang::FieldDecl* Decl) const;

		FunctionDescriptor*
		TryGetFunction(const clang::FunctionDecl* Decl) const;

		EnumDescriptor*
		TryGetEnum(const clang::EnumDecl* Decl) const;

		DescriptorId
		GetNewDescriptorId();

	private:
		struct Impl;
		Impl* m_impl;
	};
}
