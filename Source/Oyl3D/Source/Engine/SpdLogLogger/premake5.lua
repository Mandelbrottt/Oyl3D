local oyl3d = premake.modules.oyl3d

group "Oyl/Engine"

project "SpdLogLogger"; do
	language "C++"
	kind "SharedLib"

	oyl3d.project.files()

	-- reflection "On"

	uses {
		"Core",
		"SpdLog"
	}
end
