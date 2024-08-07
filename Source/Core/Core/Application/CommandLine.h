#pragma once

#include <any>
#include <functional>
#include <optional>

#include "Core/Common.h"
#include "Core/Types/Singleton.h"
#include "Core/Types/Typedefs.h"

namespace Oyl
{
	using ArbitraryData = std::pair<void*, uint32>;

	struct CommandLineArgument final
	{
		explicit
		CommandLineArgument();

		explicit
		CommandLineArgument(int32 a_value);

		explicit
		CommandLineArgument(std::string_view a_value);

		explicit
		CommandLineArgument(ArbitraryData a_value);

		explicit
		CommandLineArgument(void* a_value, uint32 a_size)
			: CommandLineArgument(ArbitraryData(a_value, a_size)) {}

		enum class ArgumentType
		{
			None,
			Integer,
			String,
			Arbitrary,
		} type;

		std::any value;
	};

	class OYL_CORE_API CommandLine final : public Singleton<CommandLine>
	{
	public:
		struct Detail
		{
			static
			void
			ParseCommandLine(size_t a_argc, const char* a_argv[])
			{
				Instance().ParseCommandLineImpl(a_argc, a_argv);
			}
		};

		static
		bool
		IsPresent(const std::string& a_name)
		{
			return Instance().IsPresentImpl(a_name);
		}

		static
		bool
		AddArgument(const std::string& a_name, bool a_overwrite = true)
		{
			return Instance().AddArgumentImpl(a_name, a_overwrite);
		}

		static
		bool
		AddInt(const std::string& a_name, int32 a_value, bool a_overwrite = true)
		{
			return Instance().AddIntImpl(a_name, a_value, a_overwrite);
		}

		static
		bool
		AddString(const std::string& a_name, std::string a_value, bool a_overwrite = true)
		{
			return Instance().AddStringImpl(a_name, a_value, a_overwrite);
		}

		static
		bool
		AddArbitraryData(const std::string& a_name, ArbitraryData a_value, bool a_overwrite = true)
		{
			return Instance().AddArbitraryDataImpl(a_name, a_value, a_overwrite);
		}

		static
		std::optional<int32>
		GetInt(const std::string& a_name)
		{
			return Instance().GetIntImpl(a_name);
		}

		static
		std::optional<std::string_view>
		GetString(const std::string& a_name)
		{
			return Instance().GetStringImpl(a_name);
		}

		static
		std::optional<ArbitraryData>
		GetArbitraryData(const std::string& a_name)
		{
			return Instance().GetArbitraryDataImpl(a_name);
		}

		static
		bool
		RemoveArgument(const std::string& a_name)
		{
			return Instance().RemoveArgumentImpl(a_name);
		}

	private:
		void
		ParseCommandLineImpl(size_t a_argc, const char* a_argv[]);

		bool
		IsPresentImpl(const std::string& a_name) const noexcept;

		bool
		AddArgumentImpl(const std::string& a_name, bool a_overwrite = true);

		bool
		AddIntImpl(const std::string& a_name, int32 a_value, bool a_overwrite = true);

		bool
		AddStringImpl(const std::string& a_name, std::string a_value, bool a_overwrite = true);

		bool
		AddArbitraryDataImpl(const std::string& a_name, ArbitraryData a_value, bool a_overwrite = true);

		std::optional<int32>
		GetIntImpl(const std::string& a_name) const;

		std::optional<std::string_view>
		GetStringImpl(const std::string& a_name) const;

		std::optional<ArbitraryData>
		GetArbitraryDataImpl(const std::string& a_name) const;

		bool
		RemoveArgumentImpl(const std::string& a_name);

		std::unordered_map<std::string, CommandLineArgument> m_arguments;
	};
}
