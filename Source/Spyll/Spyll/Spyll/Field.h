#pragma once

#include "Enums.h"

namespace Spyll::Reflection
{
	class Type;

	class Field final
	{
	public:
		~Field();

		Field(const Field&) = delete;
		Field&
		operator =(const Field&) = delete;

		Field(Field&&) = delete;
		Field&
		operator =(Field&&) = delete;

		std::string_view
		GetName() const;

		std::string_view
		GetQualifiedName() const;

		const Type*
		GetType() const;

		const Type*
		GetOwningType() const;

		uint32_t
		GetOffset() const;

		AccessSpecifier
		GetAccessSpecifier() const;

		bool
		IsConst() const;

		bool
		IsVolatile() const;

		bool
		IsPointer() const;

	private:
		Field();

		struct Impl;
		Impl* m_impl;
	};
}
