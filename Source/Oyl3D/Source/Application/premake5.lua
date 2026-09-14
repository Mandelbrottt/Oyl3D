local Project = require "Project"

group "Oyl/Executables"

startproject "Oyl.Application"

project "Oyl.Application"; do
	language "C++"
	kind "WindowedApp"

	targetname("Oyl3D%{cfg.platform}")

	Project.Files()

	reflection "On"

	links {
		"Oyl.Core",
		"Oyl.Rendering",
		"Oyl.Editor",
	}

	links {
		"SpdLog",
		"TracyClient",
		"Glfw",
	}
end
