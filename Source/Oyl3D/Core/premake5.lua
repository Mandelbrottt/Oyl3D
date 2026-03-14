group "Engine"

project "Core"; do
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
