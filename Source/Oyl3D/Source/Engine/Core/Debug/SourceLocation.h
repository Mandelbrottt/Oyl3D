#pragma once

#include "Core/Types/PrimitiveTypes.h"

namespace Oyl::Debug
{
	struct SourceLocation
	{
		// Use __builtin functions supported by every major compiler
		static
		constexpr SourceLocation
		Current(
			uint32 a_line = __builtin_LINE(),
			uint32 a_column = __builtin_COLUMN(),
			const char* a_function = __builtin_FUNCTION(),
			const char* a_file = __builtin_FILE()
		)
		{
			return SourceLocation {
				a_line,
				a_column,
				a_function,
				a_file
			};
		}

		uint32 line;
		uint32 column;
		const char* function;
		const char* file;
	};
}
