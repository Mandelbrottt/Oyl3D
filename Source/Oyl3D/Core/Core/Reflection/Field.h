#pragma once

#include "MemberInfo.h"
#include "Variable.h"

namespace Oyl::Reflection
{
	class Type;

	namespace Internal
	{
		class ReflectionFactory;
		struct FieldParams;
	}

	class Field final : public Variable, public MemberInfo
	{
		friend Internal::ReflectionFactory;

		explicit
		Field(const Internal::FieldParams& a_params);

	public:
		uint32_t
		GetOffset() const
		{
			return m_offsetInBits / 8;
		}

		uint32_t
		GetOffsetInBits() const
		{
			return m_offsetInBits;
		}

		template<typename TSelf, typename TField>
		TField&
		GetValue(const TSelf& a_self) const;

		template<typename TSelf, typename TField>
		void
		SetValue(TSelf& a_self, const TField& a_value) const;

		template<typename TSelf, typename TField>
		void
		SetValue(TSelf& a_self, TField&& a_value) const;

	private:
		bool
		GetValue(void* a_self, void** a_outPtr) const;

	private:
		uint32_t m_offsetInBits = 0;
	};

	template<typename TSelf, typename TField>
	TField&
	Field::GetValue(const TSelf& a_self) const
	{
		// TODO: Type Checking
		TField* out = nullptr;
		GetValue(&a_self, &out);
		return *std::launder(out);
	}

	template<typename TSelf, typename TField>
	void
	Field::SetValue(TSelf& a_self, const TField& a_value) const
	{
		// TODO: Type Checking
		TField& field = GetValue(a_self);
		field = a_value;
	}

	template<typename TSelf, typename TField>
	void
	Field::SetValue(TSelf& a_self, TField&& a_value) const
	{
		// TODO: Type Checking
		TField& field = GetValue(a_self);
		field = a_value;
	}
}
