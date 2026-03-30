#pragma once

#include "Enums.h"

namespace Spyll::Reflection
{
	class Type;

	class Field final
	{
		Field();

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

		bool
		IsReference() const;

		template<typename TSelf, typename TField>
		TField&
		GetValue(const TSelf& a_self) const;

		template<typename TSelf, typename TField>
		void
		SetValue(TSelf& a_self, const TField& a_value);

		template<typename TSelf, typename TField>
		void
		SetValue(TSelf& a_self, TField&& a_value);

	private:
		bool
		GetValue(void* a_self, void** a_outPtr);

	private:
		std::string m_name;
		std::string m_qualifiedName;

		Type* m_type = nullptr;

		Type* m_ownerType = nullptr;
		uint32_t m_offset = 0;

		AccessSpecifier m_accessSpecifier = AccessSpecifier::None;

		bool m_isConst = false;
		bool m_isVolatile = false;
		bool m_isReference = false;
		bool m_isPointer = false;
	};

	template<typename TSelf, typename TField>
	TField&
	Field::GetValue(const TSelf& a_self) const
	{
		// TODO: Type Checking
		TField* out;
		GetValue(&a_self, &out);
		return *std::launder(out);
	}

	template<typename TSelf, typename TField>
	void
	Field::SetValue(TSelf& a_self, const TField& a_value)
	{
		// TODO: Type Checking
		TField& field = GetValue(a_self);
		field = a_value;
	}

	template<typename TSelf, typename TField>
	void
	Field::SetValue(TSelf& a_self, TField&& a_value)
	{
		// TODO: Type Checking
		TField& field = GetValue(a_self);
		field = a_value;
	}
}
