#pragma once

#include <any>
#include <optional>

#include "Core/Containers/String.h"
#include "Core/Preprocessor/Common.h"

int
main(int, char**);

namespace Oyl
{
	struct ArbitraryData
	{
		ArbitraryData(void* a_data, uint32 a_size)
			: data(a_data),
			  size(a_size) {}

		template<typename T>
		explicit
		ArbitraryData(T* a_data)
			: data(static_cast<void*>(a_data)),
			  size(sizeof(*a_data)) {}

		void* data;
		uint32 size;
	};

	enum class CommandLineArgumentType
	{
		None,
		Integer,
		String,
		Arbitrary,
	};

	struct CORE_EXPORT CommandLineArgument final
	{
		explicit
		CommandLineArgument()
			: type(CommandLineArgumentType::None) {}

		CommandLineArgument(int32 a_value)
			: type(CommandLineArgumentType::Integer),
			  value(a_value) {}

		CommandLineArgument(StringView a_value)
			: type(CommandLineArgumentType::String),
			  value(String(a_value)) {}

		CommandLineArgument(ArbitraryData a_value)
			: type(CommandLineArgumentType::Arbitrary),
			  value(std::move(a_value)) {}

		CommandLineArgument(void* a_value, uint32 a_size)
			: CommandLineArgument(ArbitraryData(a_value, a_size)) {}

		CommandLineArgumentType type;
		std::any value;
	};

	class CORE_EXPORT CommandLine final
	{
		friend int
		::main(int, char**);

		CommandLine() = delete;

	public:
		static
		bool
		IsPresent(const String& a_name);

		static
		bool
		AddArgument(
			const String& a_name,
			CommandLineArgument a_argument = CommandLineArgument(),
			bool a_overwrite = true
		);

		static
		bool
		AddInt(const String& a_name, int32 a_value, bool a_overwrite = true);

		static
		bool
		AddString(const String& a_name, const String& a_value, bool a_overwrite = true);

		static
		bool
		AddArbitraryData(const String& a_name, ArbitraryData a_value, bool a_overwrite = true);

		static
		std::optional<int32>
		GetInt(const String& a_name);

		static
		std::optional<StringView>
		GetString(const String& a_name);

		static
		std::optional<ArbitraryData>
		GetArbitraryData(const String& a_name);

		static
		bool
		RemoveArgument(const String& a_name);

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
