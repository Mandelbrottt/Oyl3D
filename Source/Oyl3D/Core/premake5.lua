group "Engine"

project "Oyl.Core"; do
	language(premake.CPP)
	kind(premake.SHAREDLIB)

	pchheader "pch.h"
	pchsource "pch.cpp"

	links {
		"SpdLog",
		"TracyClient",
		"Vulkan",
	}
end
