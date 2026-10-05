#include "CommandLine.h"

#include <unordered_map>

#include "Core/Logging/Logging.h"

namespace Oyl
{
	struct CommandLine::Impl
	{
		std::unordered_map<std::string, CommandLineArgument> namedArguments;
	};

	CommandLine::Impl* CommandLine::s_impl = nullptr;

	bool
	CommandLine::IsPresent(const String& a_name)
	{
		const auto iter = s_impl->namedArguments.find(a_name);
		return iter != s_impl->namedArguments.end();
	}

	bool
	CommandLine::AddArgument(
		const String& a_name,
		CommandLineArgument a_argument,
		bool a_overwrite
	)
	{
		auto [iter, insert] = s_impl->namedArguments.try_emplace(a_name, a_argument);
		if (!insert && !a_overwrite)
		{
			return false;
		}

		auto& arg = iter->second;
		arg = std::move(a_argument);
		return true;
	}

	bool
	CommandLine::AddInt(const String& a_name, int32 a_value, bool a_overwrite)
	{
		auto [iter, insert] = s_impl->namedArguments.try_emplace(a_name, a_value);
		if (!insert && !a_overwrite)
		{
			return false;
		}

		auto& arg = iter->second;
		arg = CommandLineArgument(a_value);
		return true;
	}

	bool
	CommandLine::AddString(const String& a_name, const String& a_value, bool a_overwrite)
	{
		auto [iter, insert] = s_impl->namedArguments.try_emplace(a_name, a_value);
		if (!insert && !a_overwrite)
		{
			return false;
		}

		auto& arg = iter->second;
		arg = CommandLineArgument(a_value);
		return true;
	}

	bool
	CommandLine::AddArbitraryData(const String& a_name, ArbitraryData a_value, bool a_overwrite)
	{
		auto [iter, insert] = s_impl->namedArguments.try_emplace(a_name, a_value);
		if (!insert && !a_overwrite)
		{
			return false;
		}

		auto& arg = iter->second;
		arg = CommandLineArgument(a_value);
		return true;
	}

	std::optional<StringView>
	CommandLine::GetString(const String& a_name)
	{
		auto iter = s_impl->namedArguments.find(a_name);
		if (iter == s_impl->namedArguments.end() || iter->second.type != CommandLineArgumentType::String)
		{
			return {};
		}
		StringView view = std::any_cast<String>(iter->second);
		return { view };
	}

	std::optional<int32>
	CommandLine::GetInt(const String& a_name)
	{
		auto iter = s_impl->namedArguments.find(a_name);
		if (iter == s_impl->namedArguments.end() || iter->second.type != CommandLineArgumentType::Integer)
		{
			return {};
		}
		return std::any_cast<int32>(iter->second);
	}

	std::optional<ArbitraryData>
	CommandLine::GetArbitraryData(const String& a_name)
	{
		auto iter = s_impl->namedArguments.find(a_name);
		if (iter == s_impl->namedArguments.end() || iter->second.type != CommandLineArgumentType::Arbitrary)
		{
			return {};
		}
		return std::any_cast<ArbitraryData>(iter->second);
	}

	bool
	CommandLine::RemoveArgument(const String& a_name)
	{
		size_t nElementsErased = s_impl->namedArguments.erase(a_name);
		return nElementsErased != 0;
	}

	void
	CommandLine::Init(int a_argc, char** a_argv)
	{
		s_impl = new Impl;

		for (int i = 0; i < a_argc; i++)
		{
			String arg = a_argv[i];

			auto isArgument = [](StringView a_arg)
			{
				return a_arg.rfind('-', 0) != std::string::npos;
			};

			if (!isArgument(arg))
			{
				if (i == 0)
				{
					// First argument is the program name, ignore
					continue;
				}
				LogWarning("Invalid command line argument \"{}\"", arg.c_str());
				continue;
			}

			String value;

			auto namePos = arg.find_first_not_of('-');
			auto equalsPos = arg.find_first_of('=');
			if (equalsPos == std::string::npos) // Is there an equals, making this a combo name and value?
			{
				equalsPos = arg.length();
			} else if (equalsPos != arg.find_last_of('=')) // Ensure there's only 1 equals sign
			{
				LogWarning("Invalid command line argument \"{}\"", arg.c_str());
				continue;
			} else // The rest of the string is the value
			{
				value = arg.substr(equalsPos + 1);
				continue;
			}

			String name = arg.substr(namePos, equalsPos - namePos);

			// Peek the next argument, if it is a value, use it for this argument
			int nextIndex = i + 1;
			if (value.empty() && nextIndex < a_argc && !isArgument(a_argv[nextIndex]))
			{
				value = a_argv[++i];
			}

			if (value.empty())
			{
				AddArgument(name);
			} else
			{
				AddString(name, value);
			}
		}
	}

	void
	CommandLine::Shutdown()
	{
		delete s_impl;
		s_impl = nullptr;
	}
}
