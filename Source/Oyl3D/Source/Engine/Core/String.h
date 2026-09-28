#pragma once

#include <string>

namespace Oyl
{
	class StringView : public std::string_view
	{
	public:
		StringView() = default;
		StringView(const StringView& a_rhs) = default;
		StringView&
		operator =(const StringView& a_rhs) = default;

		StringView(StringView&& a_rhs) noexcept = default;

		StringView&
		operator =(StringView&& a_rhs) noexcept = default;

		StringView(const std::string_view& a_rhs) noexcept
			: std::string_view(a_rhs) {}

		StringView&
		operator =(const std::string_view& a_rhs) noexcept
		{
			std::string_view temp(a_rhs);
			std::swap<std::string_view&&>(*this, temp);
			return *this;
		}

		StringView(std::string_view&& a_rhs) noexcept
			: std::string_view(std::move(a_rhs)) {}

		StringView&
		operator =(std::string_view&& a_rhs) noexcept
		{
			if (this != &a_rhs)
				std::swap<std::string_view&&>(*this, a_rhs);
			return *this;
		}

		StringView(const char* a_rhs) noexcept
			: std::string_view(a_rhs) {}

		StringView&
		operator =(const char* a_rhs) noexcept
		{
			std::string_view temp(a_rhs);
			std::swap<std::string_view>(*this, temp);
			return *this;
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
