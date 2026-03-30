#pragma once

#include "Enums.h"

namespace Spyll::Reflection
{
	class Type;

	class Function final
	{
	public:
		~Function();

		Function(const Function&) = delete;
		Function&
		operator =(const Function&) = delete;

		Function(Function&&) = delete;
		Function&
		operator =(Function&&) = delete;

		std::string_view
		GetName() const;

		std::string_view
		GetQualifiedName() const;

		const Type*
		GetReturnType() const;

		bool
		IsMember() const;

		const Type*
		GetOwningType() const;

		bool
		IsConstructor() const;

		ConstructorType
		GetConstructorType() const;

		bool
		IsVirtual() const;

		bool
		IsPureVirtual() const;

		bool
		IsOverride() const;

		bool
		IsStatic() const;

		bool
		IsConst() const;

		bool
		IsVolatile() const;

		bool
		IsReturnConst() const;

		bool
		IsReturnVolatile() const;

		bool
		IsReturnPointer() const;

	private:
		Function();

		struct Impl;
		Impl* m_impl;
	};
}
