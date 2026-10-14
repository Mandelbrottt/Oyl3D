local oyl3d = premake.modules.oyl3d

group "Oyl/Engine"

project "GlfwGraphics"; do
	language "C++"
	kind "SharedLib"

	oyl3d.project.files()

	-- reflection "On"

	uses {
		"Core",
		"Graphics",
		"Glfw",
	}
	usage "PUBLIC"; do
		uses {
			"Core",
			"Graphics"
		}
	end
end
