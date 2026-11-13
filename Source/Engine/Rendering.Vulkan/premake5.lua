local oyl3d = premake.modules.oyl3d

group "Oyl/Engine"

project "Rendering.Vulkan"; do
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
			"VulkanSdk",
		}
	end
end
