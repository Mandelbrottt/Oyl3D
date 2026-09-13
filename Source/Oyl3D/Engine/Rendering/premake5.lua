local Oyl3D = require "Oyl3D"

group "Oyl/Engine"

Oyl3D.CppProject "Oyl.Rendering"; do
	kind "SharedLib"

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
