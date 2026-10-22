local oyl3d = premake.modules.oyl3d

group "Oyl/Engine"

project "Core"; do
	language "C++"
	kind "SharedLib"

	oyl3d.project.files()

	-- reflection "On"

	usage "PUBLIC"; do
		uses {
			"Tracy",
			"ImGui",
		}
	end
end
