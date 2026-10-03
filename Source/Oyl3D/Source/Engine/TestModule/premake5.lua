local oyl3d = premake.modules.oyl3d

group "Oyl/Engine"

project "Test"; do
	language "C++"
	kind "SharedLib"

	oyl3d.project.files()

	-- reflection "On"
	
	uses {
		"Core"
	}
end
