local Oyl3D = require "Oyl3D"
local Engine = require "Engine"

group "Oyl/Executables"

startproject "Oyl.Application"

Oyl3D.CppProject "Oyl.Application"; do
	kind "WindowedApp"

	targetname(Oyl3D.Name .. "%{cfg.platform}")

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
