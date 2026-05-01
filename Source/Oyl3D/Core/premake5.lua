local Engine = require "Engine"

group "Engine"

project "Oyl.Core"; do
	language "C++"
	kind "SharedLib"

	Engine.GenerateOylSpyllInformation()

	links {
		"SpdLog",
		"TracyClient",
		"Vulkan",
	}

	dependson {
		"Oyl.Spyll",
	}
end
