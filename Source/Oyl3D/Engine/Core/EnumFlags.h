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
			return EnumFlags(m_mask | a_rhs.m_mask);
		}

		constexpr EnumFlags
		operator &(EnumFlags a_rhs) const
		{
			return EnumFlags(m_mask & a_rhs.m_mask);
		}

		constexpr EnumFlags
		operator ^(EnumFlags a_rhs) const
		{
			return EnumFlags(m_mask ^ a_rhs.m_mask);
		}

		constexpr EnumFlags
		operator ~() const
		{
			return EnumFlags(~m_mask);
		}

		constexpr EnumFlags&
		operator =(const EnumFlags& a_rhs) = default;

		constexpr EnumFlags&
		operator |=(EnumFlags a_rhs)
		{
			return (*this = *this | a_rhs);
		}

		constexpr EnumFlags&
		operator &=(EnumFlags a_rhs)
		{
			return (*this = *this & a_rhs);
		}

		constexpr EnumFlags&
		operator ^=(EnumFlags a_rhs)
		{
			return (*this = *this ^ a_rhs);
		}

		constexpr EnumFlags
		operator |(TBits a_rhs) const noexcept
		{
			return EnumFlags(m_mask | static_cast<TMask>(a_rhs));
		}

		constexpr EnumFlags
		operator &(TBits a_rhs) const
		{
			return EnumFlags(m_mask & static_cast<TMask>(a_rhs));
		}

		constexpr EnumFlags
		operator ^(TBits a_rhs) const
		{
			return EnumFlags(m_mask ^ static_cast<TMask>(a_rhs));
		}

		constexpr EnumFlags&
		operator =(TBits a_rhs)
		{
			*this = EnumFlags(a_rhs);
			return *this;
		}

		constexpr EnumFlags&
		operator |=(TBits a_rhs)
		{
			return (*this = *this | a_rhs);
		}

		constexpr EnumFlags&
		operator &=(TBits a_rhs)
		{
			return (*this = *this & a_rhs);
		}

		constexpr EnumFlags&
		operator ^=(TBits a_rhs)
		{
			return (*this = *this ^ a_rhs);
		}

		constexpr EnumFlags
		friend operator |(TBits a_lhs, EnumFlags a_rhs) noexcept
		{
			return EnumFlags(a_lhs) | a_rhs;
		}

		constexpr EnumFlags
		friend operator &(TBits a_lhs, EnumFlags a_rhs) noexcept
		{
			return EnumFlags(a_lhs) & a_rhs;
		}

		constexpr EnumFlags
		friend operator ^(TBits a_lhs, EnumFlags a_rhs) noexcept
		{
			return EnumFlags(a_lhs) ^ a_rhs;
		}

		explicit
		constexpr
		operator bool() const
		{
			return !!m_mask;
		}

		explicit
		constexpr
		operator TMask() const
		{
			return m_mask;
		}

	private:
		TMask m_mask = {};
	};
}

#define OYL_ENUM_CLASS_BITWISE_OPERATIONS(_enum_class_) \
	constexpr CommandQueueFlagBits \
	operator |(CommandQueueFlagBits a_lhs, CommandQueueFlagBits a_rhs) \
	{ \
		auto mask = static_cast<Traits::TUnderlyingType<CommandQueueFlagBits>>(a_lhs) \
		            | static_cast<Traits::TUnderlyingType<CommandQueueFlagBits>>(a_rhs); \
		return static_cast<CommandQueueFlagBits>(mask); \
	} \
	\
	constexpr CommandQueueFlagBits \
	operator &(CommandQueueFlagBits a_lhs, CommandQueueFlagBits a_rhs) \
	{ \
		auto mask = static_cast<Traits::TUnderlyingType<CommandQueueFlagBits>>(a_lhs) \
		            & static_cast<Traits::TUnderlyingType<CommandQueueFlagBits>>(a_rhs);\
		return static_cast<CommandQueueFlagBits>(mask); \
	} \
	\
	constexpr CommandQueueFlagBits \
	operator ^(CommandQueueFlagBits a_lhs, CommandQueueFlagBits a_rhs) \
	{ \
		auto mask = static_cast<Traits::TUnderlyingType<CommandQueueFlagBits>>(a_lhs) \
		            ^ static_cast<Traits::TUnderlyingType<CommandQueueFlagBits>>(a_rhs); \
		return static_cast<CommandQueueFlagBits>(mask); \
	}
