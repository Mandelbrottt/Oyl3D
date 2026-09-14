local oyl3d = premake.modules.oyl3d

group "Oyl/Engine"

project "Oyl.Core"; do
	language "C++"
	kind "SharedLib"

	oyl3d.project.files()

	reflection "On"

	links {
		"SpdLog",
		"TracyClient",
		"Glfw",
	}
end
