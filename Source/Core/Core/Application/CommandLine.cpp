#include "pch.h"
#include "CommandLine.h"

namespace Oyl
{
	CommandLineArgument::CommandLineArgument()
		: type { ArgumentType::None } { }

	CommandLineArgument::CommandLineArgument(i32 a_value)
		: type { ArgumentType::Integer },
		  value { a_value } { }

	CommandLineArgument::CommandLineArgument(std::string_view a_value)
		: type { ArgumentType::String },
		  value { std::string { a_value } } { }

	CommandLineArgument::CommandLineArgument(ArbitraryData a_value)
		: type { ArgumentType::Arbitrary },
		  value { a_value } { }

	bool
	CommandLine::IsPresentImpl(const std::string& a_name) const noexcept
	{
		const auto iter = m_arguments.find(a_name);
		return iter != m_arguments.end();
	}

	bool
	CommandLine::AddStringImpl(const std::string& a_name, std::string a_value, bool a_overwrite)
	{
		auto [iter, insert] = m_arguments.emplace(a_name, CommandLineArgument {});
		if (!insert && !a_overwrite)
		{
			return false;
		}

		auto& arg = iter->second;
		arg.type  = CommandLineArgument::ArgumentType::String;
		arg.value = a_value;
		return true;
	}

	bool
	CommandLine::AddIntImpl(const std::string& a_name, i32 a_value, bool a_overwrite)
	{
		auto [iter, insert] = m_arguments.emplace(a_name, CommandLineArgument {});
		if (!insert && !a_overwrite)
		{
			return false;
		}

		auto& arg = iter->second;
		arg.type  = CommandLineArgument::ArgumentType::Integer;
		arg.value = a_value;
		return true;
	}

	bool
	CommandLine::AddArbitraryDataImpl(const std::string& a_name, ArbitraryData a_value, bool a_overwrite)
	{
		auto [iter, insert] = m_arguments.emplace(a_name, CommandLineArgument {});
		if (!insert && !a_overwrite)
		{
			return false;
		}

		auto& arg = iter->second;
		arg.type  = CommandLineArgument::ArgumentType::Arbitrary;
		arg.value = a_value;
		return true;
	}

	std::optional<std::string_view>
	CommandLine::GetStringImpl(const std::string& a_name) const
	{
		auto iter = m_arguments.find(a_name);
		if (iter == m_arguments.end() || iter->second.type != CommandLineArgument::ArgumentType::String)
		{
			return {};
		}
		return { std::any_cast<std::string>(iter->second) };
	}

	std::optional<i32>
	CommandLine::GetIntImpl(const std::string& a_name) const
	{
		auto iter = m_arguments.find(a_name);
		if (iter == m_arguments.end() || iter->second.type != CommandLineArgument::ArgumentType::Integer)
		{
			return {};
		}
		return std::any_cast<i32>(iter->second);
	}

	std::optional<ArbitraryData>
	CommandLine::GetArbitraryDataImpl(const std::string& a_name) const
	{
		auto iter = m_arguments.find(a_name);
		if (iter == m_arguments.end() || iter->second.type != CommandLineArgument::ArgumentType::Arbitrary)
		{
			return {};
		}
		return std::any_cast<ArbitraryData>(iter->second);
	}
}
