#pragma once

#include "Variable.h"

namespace Oyl::Reflection
{
	class Type;

	namespace Internal
	{
		class ReflectionFactory;
		struct FieldParams;
	}

	class Field final : public Variable
	{
		friend Internal::ReflectionFactory;

		explicit
		Field(const Internal::FieldParams& a_params);

	public:
		uint32
		GetOffset() const
		{
			return m_offset;
		}

		uint32
		GetOffsetInBits() const
		{
			return m_offsetInBits;
		}

		bool
		IsConst() const
		{
			return m_isConst;
		}

		template<typename TSelf, typename TField>
		TField&
		GetValueUnsafe(const TSelf& a_self) const;

		template<typename TSelf, typename TField>
		void
		SetValueUnsafe(TSelf& a_self, const TField& a_value) const;

		template<typename TSelf, typename TField>
		void
		SetValueUnsafe(TSelf& a_self, TField&& a_value) const;

	private:
		bool
		GetValueUnsafe(void* a_self, void** a_outPtr) const;

	private:
		uint16_t m_offset;
		uint16_t m_offsetInBits;

		bool m_isConst;
	};

	template<typename TSelf, typename TField>
	TField&
	Field::GetValueUnsafe(const TSelf& a_self) const
	{
		// TODO: Type Checking
		TField* out = nullptr;
		GetValue(&a_self, &out);
		return *std::launder(out);
	}

	template<typename TSelf, typename TField>
	void
	Field::SetValueUnsafe(TSelf& a_self, const TField& a_value) const
	{
		// TODO: Type Checking
		TField& field = GetValueUnsafe(a_self);
		field = a_value;
	}

	template<typename TSelf, typename TField>
	void
	Field::SetValueUnsafe(TSelf& a_self, TField&& a_value) const
	{
		// TODO: Type Checking
		TField& field = GetValueUnsafe(a_self);
		field = a_value;
	}
}
