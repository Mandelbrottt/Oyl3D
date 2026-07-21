local Engine = require "Engine"

group "Oyl/Engine"

project "Oyl.Rendering"; do
	language "C++"
	kind "SharedLib"

	-- Engine.GenerateOylSpyllInformation()

	links {
		"Oyl.Core",
	}

	links {
		"SpdLog",
		"TracyClient",
		"Glfw",
		"Vulkan",
	}
end
