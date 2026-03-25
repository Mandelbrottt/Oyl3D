#pragma once

#include "Spyll/ClangTool/ReflectionGenerator.h"
#include "Spyll/Spyll/Descriptors/ReflectionDescriptor.h"

class RecordsTest : public testing::Test
{
	static const char* s_compileTarget;

protected:
	const Spyll::TestTool* tool;
	const Spyll::ReflectionDescriptor* descriptor;

	RecordsTest()
	{
		static const Spyll::TestTool* staticTool = RunTool();
		static const Spyll::ReflectionDescriptor staticDescriptor = Descriptor(staticTool);

		tool = staticTool;
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
	static
	const Spyll::TestTool*
	RunTool()
	{
		auto* tool = new Spyll::TestTool(s_compileTarget, "-x c++");
		tool->Run();
		return tool;
	}

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
