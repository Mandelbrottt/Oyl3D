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
	constexpr _enum_class_ \
	operator |(_enum_class_ a_lhs, _enum_class_ a_rhs) \
	{ \
		auto mask = static_cast<Traits::TUnderlyingType<_enum_class_>>(a_lhs) \
		            | static_cast<Traits::TUnderlyingType<_enum_class_>>(a_rhs); \
		return static_cast<_enum_class_>(mask); \
	} \
	\
	constexpr _enum_class_ \
	operator &(_enum_class_ a_lhs, _enum_class_ a_rhs) \
	{ \
		auto mask = static_cast<Traits::TUnderlyingType<_enum_class_>>(a_lhs) \
		            & static_cast<Traits::TUnderlyingType<_enum_class_>>(a_rhs);\
		return static_cast<_enum_class_>(mask); \
	} \
	\
	constexpr _enum_class_ \
	operator ^(_enum_class_ a_lhs, _enum_class_ a_rhs) \
	{ \
		auto mask = static_cast<Traits::TUnderlyingType<_enum_class_>>(a_lhs) \
		            ^ static_cast<Traits::TUnderlyingType<_enum_class_>>(a_rhs); \
		return static_cast<_enum_class_>(mask); \
	}

#define OYL_DEFINE_ENUM_FLAGS_CONVERSION_FUNCTIONS( \
	_src_function_name_, \
	_dst_function_name_, \
	_src_flags_type_, \
	_dst_flags_type_, \
	_src_flag_bits_type_, \
	_dst_flag_bits_type_, \
	... \
) \
	inline \
	_src_flags_type_ \
	_src_function_name_(_dst_flags_type_ a_flags) \
	{ \
		_src_flags_type_ result {}; \
		\
		using pair = std::pair<_src_flag_bits_type_, _dst_flag_bits_type_>; \
		using init_list = std::initializer_list<pair>; \
		\
		auto checkFlagBit = [&result, &a_flags](const init_list& a_list) \
		{ \
			for (auto& [srcBit, dstBit] : a_list) \
			{ \
				if (!(a_flags & dstBit)) \
					continue; \
				\
				result |= srcBit; \
				\
				OYL_STRIP_IN_DISTRIBUTION(a_flags &= ~((_dst_flags_type_) dstBit)); \
			} \
		}; \
		checkFlagBit({ __VA_ARGS__ }); \
		\
		return result; \
	} \
	\
	inline \
	_dst_flags_type_ \
	_dst_function_name_(_src_flags_type_ a_flags) \
	{ \
		_dst_flags_type_ result {}; \
		\
		using pair = std::pair<_src_flag_bits_type_, _dst_flag_bits_type_>; \
		using init_list = std::initializer_list<pair>; \
		\
		auto checkFlagBit = [&result, &a_flags](const init_list& a_list) \
		{ \
			for (auto& [srcBit, dstBit] : a_list) \
			{ \
				if (!(a_flags & srcBit)) \
					continue; \
				\
				result |= dstBit; \
				\
				OYL_STRIP_IN_DISTRIBUTION(a_flags &= ~((_src_flags_type_) srcBit)); \
			} \
		}; \
		checkFlagBit({ __VA_ARGS__ }); \
		\
		OYL_ASSERT(!a_flags, "Missing " #_src_flag_bits_type_ " check!"); \
		return result; \
	}

#define OYL_DEFINE_ENUM_CONVERSION_FUNCTIONS( \
	_src_function_name_, \
	_dst_function_name_, \
	_src_enum_type_, \
	_dst_enum_type_, \
	... \
) \
	inline \
	_src_enum_type_ \
	_src_function_name_(_dst_enum_type_ a_dst) \
	{ \
		using pair = std::pair<_src_enum_type_, _dst_enum_type_>; \
		using init_list = std::initializer_list<pair>; \
		\
		auto checkFlagBit = [&a_dst](const init_list& a_list) -> _src_enum_type_ \
		{ \
			for (auto& [src, dst] : a_list) \
			{ \
				if (dst != a_dst) \
					continue; \
				\
				return src; \
			} \
			return {}; \
		}; \
		auto result = checkFlagBit({ __VA_ARGS__ }); \
		return result; \
	} \
	\
	inline \
	_dst_enum_type_ \
	_dst_function_name_(_src_enum_type_ a_src) \
	{ \
		using pair = std::pair<_src_enum_type_, _dst_enum_type_>; \
		using init_list = std::initializer_list<pair>; \
		\
		auto checkFlagBit = [&a_src](const init_list& a_list) -> _dst_enum_type_ \
		{ \
			for (auto& [src, dst] : a_list) \
			{ \
				if (src != a_src) \
					continue; \
				\
				return dst; \
			} \
			return {}; \
		}; \
		auto result = checkFlagBit({ __VA_ARGS__ }); \
		return result; \
	}
