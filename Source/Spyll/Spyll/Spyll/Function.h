#pragma once

#include "Enums.h"

namespace Spyll::Reflection
{
	class Type;
	class Assembly;

	class Function final
	{
		friend
		void
		::_PopulateReflectionAssembly(Assembly* a_assembly);

		Function();

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
		std::string m_name;
		std::string m_qualifiedName;

		Type* m_returnType = nullptr;

		Type* m_ownerType = nullptr;

		AccessSpecifier m_accessSpecifier = AccessSpecifier::None;
		ConstructorType m_constructorType = ConstructorType::None;

		bool m_isVirtual = false;
		bool m_isPureVirtual = false;
		bool m_isOverride = false;

		bool m_isStatic = false;
		bool m_isConst = false;
		bool m_isVolatile = false;

		bool m_isReturnConst = false;
		bool m_isReturnVolatile = false;
		bool m_isReturnReference = false;
		bool m_isReturnPointer = false;
	};
}
