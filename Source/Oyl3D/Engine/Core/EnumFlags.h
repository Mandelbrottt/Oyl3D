#pragma once

#include "TypeTraits.h"

namespace Oyl
{
	template<Traits::Enum TBits>
	class EnumFlags
	{
	public:
		using TMask = Traits::TUnderlyingType<TBits>;

		constexpr
		EnumFlags() noexcept = default;

		constexpr
		EnumFlags(TBits a_bits) noexcept
			: m_mask(static_cast<TMask>(a_bits)) {}

		explicit
		constexpr
		EnumFlags(TMask a_mask) noexcept
			: m_mask(a_mask) {}

		EnumFlags(const EnumFlags&) noexcept = default;
		EnumFlags(EnumFlags&&) noexcept = default;

		constexpr
		auto
		operator <=>(const EnumFlags& a_rhs) const noexcept = default;

		constexpr EnumFlags
		operator |(EnumFlags a_rhs) const noexcept
		{
			return EnumFlags(m_mask | a_rhs);
		}

		constexpr EnumFlags
		operator &(EnumFlags a_rhs) const
		{
			return EnumFlags(m_mask & a_rhs);
		}

		constexpr EnumFlags
		operator ^(EnumFlags a_rhs) const
		{
			return EnumFlags(m_mask ^ a_rhs);
		}

		constexpr EnumFlags
		operator ~() const
		{
			return EnumFlags(~0);
		}

		constexpr EnumFlags
		operator =(EnumFlags a_rhs) const = default;

		constexpr EnumFlags&
		operator |=(EnumFlags a_rhs) const
		{
			return (*this = *this | a_rhs);
		}

		constexpr EnumFlags&
		operator &=(EnumFlags a_rhs) const
		{
			return (*this = *this & a_rhs);
		}

		constexpr EnumFlags&
		operator ^=(EnumFlags a_rhs) const
		{
			return (*this = *this ^ a_rhs);
		}

		explicit
		constexpr operator bool() const
		{
			return !!m_mask;
		}

		explicit
		constexpr operator TMask() const
		{
			return m_mask;
		}

	private:
		TMask m_mask = {};
	};
}
