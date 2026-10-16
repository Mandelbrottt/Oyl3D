#pragma once

#include <format>
#include <string>

#include "Core/Types/PrimitiveTypes.h"
#include "Core/Types/TypeTraits.h"

namespace Oyl
{
	class StringView : public std::string_view
	{
	public:
		constexpr
		StringView() = default;

		constexpr
		StringView(const StringView& a_rhs) = default;

		constexpr
		StringView&
		operator =(const StringView& a_rhs) = default;

		constexpr
		StringView(StringView&& a_rhs) noexcept = default;

		constexpr
		StringView&
		operator =(StringView&& a_rhs) noexcept = default;

		constexpr
		StringView(const std::string_view& a_rhs) noexcept
			: std::string_view(a_rhs) {}

		constexpr
		StringView&
		operator =(const std::string_view& a_rhs) noexcept
		{
			std::string_view temp(a_rhs);
			std::swap<std::string_view&&>(*this, temp);
			return *this;
		}

		constexpr
		StringView(std::string_view&& a_rhs) noexcept
			: std::string_view(std::move(a_rhs)) {}

		constexpr
		StringView&
		operator =(std::string_view&& a_rhs) noexcept
		{
			if (this != &a_rhs)
				std::swap<std::string_view&&>(*this, a_rhs);
			return *this;
		}

		constexpr
		StringView(const char* a_rhs) noexcept
			: std::string_view(a_rhs) {}

		constexpr
		StringView(const char* a_rhs, uint a_length) noexcept
			: std::string_view(a_rhs, a_length) {}

		constexpr
		StringView&
		operator =(const char* a_rhs) noexcept
		{
			std::string_view temp(a_rhs);
			std::swap<std::string_view>(*this, temp);
			return *this;
		}

		constexpr const char*
		Data() const
		{
			return std::string_view::data();
		}
	};

	class String : public std::string
	{
	public:
		String() = default;
		String(const String& a_rhs) = default;
		String&
		operator =(const String& a_rhs) = default;

		String(String&& a_rhs) noexcept = default;

		String&
		operator =(String&& a_rhs) noexcept = default;

		friend
		String
		operator +(const String& a_lhs, const String& a_rhs)
		{
			return String((const std::string&) a_lhs + (const std::string&) a_rhs);
		}

		friend
		String
		operator +(const char* a_lhs, const String& a_rhs)
		{
			return String(a_lhs + (const std::string&) a_rhs);
		}

		friend
		String
		operator +(const String& a_lhs, const char* a_rhs)
		{
			return String((const std::string&) a_lhs + a_rhs);
		}

		String&
		operator +=(const String& a_rhs)
		{
			return *this = (*this + a_rhs);
		}

		String(const char* a_rhs)
			: std::string(a_rhs) {}

		String&
		operator =(const char* a_rhs)
		{
			std::string temp(a_rhs);
			std::swap(*this, temp);
			return *this;
		}

		explicit
		String(uint32 a_length, char a_char = '\0')
		{
			resize(a_length, a_char);
		}

		String(const char* a_str, uint32 a_length)
			: std::string(a_str, a_length) {}

		String(const std::string& a_rhs)
			: std::string(a_rhs) {}

		String&
		operator =(const std::string& a_rhs)
		{
			std::string temp(a_rhs);
			std::swap(*this, temp);
			return *this;
		}

		String(std::string&& a_rhs) noexcept
			: std::string(std::move(a_rhs)) {}

		String&
		operator =(std::string&& a_rhs) noexcept
		{
			if (this != &a_rhs)
				std::swap(*this, a_rhs);
			return *this;
		}

		explicit
		String(std::string_view a_rhs)
			: std::string(a_rhs) {}

		String&
		operator =(std::string_view a_rhs)
		{
			std::string temp(a_rhs);
			std::swap(*this, temp);
			return *this;
		}

		constexpr
		operator StringView() const noexcept
		{
			return StringView(c_str(), (uint32) length());
		}
	};
}

template<>
struct std::formatter<Oyl::String>
	: std::formatter<std::string>
{
	auto
	format(
		const Oyl::String& a_string,
		std::format_context& a_ctx
	) const -> decltype(a_ctx.out())
	{
		return format_to(a_ctx.out(), "{}", static_cast<const std::string&>(a_string));
	}
};

template<>
struct std::formatter<Oyl::StringView>
	: std::formatter<std::string_view>
{
	auto
	format(
		Oyl::StringView a_view,
		std::format_context& a_ctx
	) const -> decltype(a_ctx.out())
	{
		return format_to(a_ctx.out(), "{}", static_cast<const std::string_view&>(a_view));
	}
};
