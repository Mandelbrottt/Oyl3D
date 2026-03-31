#pragma once

#include "MemberInfo.h"
#include "Variable.h"

namespace Spyll::Reflection
{
	class Type;

	class Field final : public Variable, public MemberInfo
	{
		friend AssemblyFactory;

	protected:
		Field() = default;

	public:
		uint32_t
		GetOffset() const
		{
			return m_offset;
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
		uint32_t m_offset = 0;
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
