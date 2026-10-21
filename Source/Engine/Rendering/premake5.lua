local oyl3d = premake.modules.oyl3d

group "Oyl/Engine"

project "Rendering"; do
	language "C++"
	kind "SharedLib"

	oyl3d.project.files()

	-- reflection "On"

	uses {
		"Core",
		"Application",
	}
	usage "PUBLIC"; do
		uses {
			"Core",
			"Application"
		}
	end
end
