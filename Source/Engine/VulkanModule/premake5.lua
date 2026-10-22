local oyl3d = premake.modules.oyl3d

group "Oyl/Engine"

project "VulkanModule"; do
	language "C++"
	kind "SharedLib"

	oyl3d.project.files()

	-- reflection "On"

	uses {
		"Core",
		"Application",
		"Glfw"
	}

	usage "PUBLIC"; do
		uses {
			"Rendering",
			"Vulkan",
		}
	end
end
