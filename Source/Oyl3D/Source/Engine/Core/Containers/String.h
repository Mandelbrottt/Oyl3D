#pragma once

#include <string>

#include "Core/Types/PrimitiveTypes.h"

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

		const char*
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
		operator +(String a_lhs, const String& a_rhs)
		{
			return String((const std::string&) a_lhs + (const std::string&) a_rhs);
		}

		String&
		operator +=(const String& a_rhs)
		{
			return *this = (*this + a_rhs);
		}

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
		String(std::string_view a_rhs)
			: std::string(a_rhs) {}

		String&
		operator =(std::string_view a_rhs)
		{
			std::string temp(a_rhs);
			std::swap(*this, temp);
			return *this;
		}

		operator StringView() const
		{
			return StringView(this->c_str());
		}
	};
}
