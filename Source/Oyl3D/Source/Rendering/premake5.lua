local oyl3d = premake.modules.oyl3d

group "Oyl/Engine"

project "Oyl.Rendering"; do
	language "C++"
	kind "SharedLib"

	oyl3d.project.files()

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
