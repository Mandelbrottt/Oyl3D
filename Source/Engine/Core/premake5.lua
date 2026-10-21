local oyl3d = premake.modules.oyl3d

group "Oyl/Engine"

project "Core"; do
	language "C++"
	kind "SharedLib"

	oyl3d.project.files()

	-- reflection "On"

	uses { "Tracy" } -- TEMP: Fix uses in usage block not being picked up by oyl project gen
	usage "PUBLIC"; do
		uses {
			"Tracy"
		}
	end
end
