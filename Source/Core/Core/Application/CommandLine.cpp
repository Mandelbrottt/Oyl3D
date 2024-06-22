#include "pch.h"
#include "CommandLine.h"

#include "Core/Logging/Logging.h"

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

	void
	CommandLine::ParseCommandLineImpl(size_t a_argc, const char* a_argv[])
	{
		std::string name;
		std::string value;
		for (int i = 0; i < a_argc; i++)
		{
			std::string arg        = a_argv[i];
			bool        isArgument = arg.rfind('-', 0) != std::string::npos;

			// If we're parsing an argument name, and the last argument was also a name,
			// it must have been an empty argument
			if (isArgument && !name.empty())
			{
				AddArgumentImpl(name);
				name = {};
			}

			if (isArgument)
			{
				auto namePos   = arg.find_first_not_of('-');
				auto equalsPos = arg.find_first_of('=');
				if (equalsPos == std::string::npos) // Is there an equals, making this a combo name and value?
				{
					equalsPos = arg.length();
				} else if (equalsPos != arg.find_last_of('=')) // Ensure there's only 1 equals sign
				{
					OYL_LOG_WARNING("Invalid command line argument argument \"{}\"", arg);
					continue;
				} else // The rest of the string is the value
				{
					value = arg.substr(equalsPos + 1);
				}

				name = arg.substr(namePos, equalsPos - namePos);
			} else if (!name.empty()) // The argument is just the value of the last argument name
			{
				value = arg;
			}

			if (name.empty() && !value.empty())
			{
				OYL_LOG_WARNING("Invalid command line value \"{}\" with no corresponding flag");
				continue;
			}

			// If the value is not empty, we already have the name and can add a name/value pair
			if (!value.empty())
			{
				AddStringImpl(name, value);
				name  = {};
				value = {};
			}
		}
	}

	bool
	CommandLine::IsPresentImpl(const std::string& a_name) const noexcept
	{
		const auto iter = m_arguments.find(a_name);
		return iter != m_arguments.end();
	}

	bool
	CommandLine::AddArgumentImpl(const std::string& a_name, bool a_overwrite)
	{
		auto [iter, insert] = m_arguments.emplace(a_name, CommandLineArgument {});
		if (!insert && !a_overwrite)
		{
			return false;
		}

		auto& arg = iter->second;
		arg.type  = CommandLineArgument::ArgumentType::None;
		arg.value = {};
		return true;
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

	bool
	CommandLine::RemoveArgumentImpl(const std::string& a_name)
	{
		size_t nElementsErased = m_arguments.erase(a_name);
		return nElementsErased != 0;
	}
}
