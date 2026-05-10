#pragma once

#include <string>

namespace Spyll {
	struct ReflectionDescriptor;
}

namespace Oyl {
	struct SpyllTool;
}

void
EmitCodeFromTool(const Oyl::SpyllTool& a_tool);