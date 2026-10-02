local oyl3d = premake.modules.oyl3d

group "Oyl/Engine"

project "Core"; do
	language "C++"
	kind "SharedLib"

	oyl3d.project.files()

	-- reflection "On"

	-- links {
	-- 	"SpdLog"
	-- }
	uses {
		"SpdLog"
	}
end
