local Oyl3D = require "Oyl3D"

group "Oyl/Engine"

Oyl3D.CppProject "Oyl.Core"; do
	kind "SharedLib"

	reflection "On"

	links {
		"SpdLog",
		"TracyClient",
		"Glfw",
	}
end
