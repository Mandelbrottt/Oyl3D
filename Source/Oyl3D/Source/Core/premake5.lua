local Project = require "Project"

group "Oyl/Engine"

project "Oyl.Core"; do
	language "C++"
	kind "SharedLib"

	Project.Files()

	reflection "On"

	links {
		"SpdLog",
		"TracyClient",
		"Glfw",
	}
end
