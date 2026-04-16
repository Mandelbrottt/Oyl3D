local Engine = require "Engine"

group "Engine"

project "Oyl.Core"; do
	language(premake.CPP)
	kind(premake.SHAREDLIB)

	Engine.GenerateOylSpyllInformation()

	links {
		"SpdLog",
		"TracyClient",
		"Vulkan",
		"Oyl.Spyll",
	}
end
