local Engine = require "Engine"

group "Executables"

startproject "Oyl.Application"

project "Oyl.Application"; do
	language(premake.CPP)
	kind(premake.WINDOWEDAPP)

	targetname(Engine.Name .. "%{cfg.platform}")

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
