local Project = require "Project"

group "Oyl/Engine"

project "Oyl.Rendering"; do
	language "C++"
	kind "SharedLib"

	Project.Files()

	reflection "On"

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
