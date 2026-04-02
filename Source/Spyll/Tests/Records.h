#pragma once

#include "ToolPreTest.h"

#include "Tool/Clang/ReflectionGenerator.h"
#include "Tool/Core/ReflectionDescriptor.h"

class Records : public ToolPreTest<Records>
{
	static const char* s_compileTarget;

protected:
	const Spyll::ReflectionDescriptor* descriptor;

	Records()
		: ToolPreTest(s_compileTarget)
	{
		static const Spyll::ReflectionDescriptor staticDescriptor = Descriptor(tool);

		descriptor = &staticDescriptor;
	}

	const Spyll::TypeDescriptor*
	TryGetTypeWithName(std::string_view a_name)
	{
		const auto begin = descriptor->types.begin();
		const auto end = descriptor->types.end();

		const auto iter =
			std::find_if(
				begin,
				end,
				[&a_name](const decltype(descriptor->types)::value_type& desc)
				{
					return desc.name == a_name;
				}
			);

		return iter != end
				   ? &(*iter)
				   : nullptr;
	}

private:
	Spyll::ReflectionDescriptor
	Descriptor(const Spyll::SpyllTool* a_tool) const
	{
		auto& map = a_tool->GetReflectionGeneratorMap();
		for (const auto& [path, generator] : map)
		{
			if (path.find(s_compileTarget) != std::string::npos)
			{
				return generator->GetReflectionDescriptor();
			}
		}
		throw "No matching generator found!";
	}
};
