#pragma once

#include <any>
#include <functional>
#include <optional>

#include "Core/Common.h"
#include "Core/Typedefs.h"

int Main(int, char**);

namespace Oyl
{
	using ArbitraryData = std::pair<void*, uint32>;

	struct OYL_CORE_API CommandLineArgument final
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

	class OYL_CORE_API CommandLine final
	{
		friend int ::Main(int, char**);

		CommandLine() = default;

	public:
		static
		bool
		IsPresent(const std::string& a_name);

		static
		bool
		AddArgument(const std::string& a_name, bool a_overwrite = true);

		static
		bool
		AddInt(const std::string& a_name, int32 a_value, bool a_overwrite = true);

		static
		bool
		AddString(const std::string& a_name, std::string a_value, bool a_overwrite = true);

		static
		bool
		AddArbitraryData(const std::string& a_name, ArbitraryData a_value, bool a_overwrite = true);

		static
		std::optional<int32>
		GetInt(const std::string& a_name);

		static
		std::optional<std::string_view>
		GetString(const std::string& a_name);

		static
		std::optional<ArbitraryData>
		GetArbitraryData(const std::string& a_name);

		static
		bool
		RemoveArgument(const std::string& a_name);

	private:
		static
		void
		Init(int a_argc, char** a_argv);

		static
		void
		Shutdown();

		struct Impl;
		static Impl* s_impl;
	};
}
