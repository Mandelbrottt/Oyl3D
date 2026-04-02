group "Engine"

project "Oyl.Core"; do
	language(premake.CPP)
	kind(premake.SHAREDLIB)

	links {
		"SpdLog",
		"TracyClient",
		"Vulkan",
		"Spyll",
	}
end
