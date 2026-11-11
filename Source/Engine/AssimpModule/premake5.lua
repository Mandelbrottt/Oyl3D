local oyl3d = premake.modules.oyl3d

group "Oyl/Engine"

project "AssimpModule"; do
	language "C++"
	kind "SharedLib"

	oyl3d.project.files()

	-- reflection "On"

	uses {
		"Assimp"
	}

	usage "PUBLIC"; do
		uses {
			"Rendering",
		}
	end
end
