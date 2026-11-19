local oyl3d = premake.modules.oyl3d

group "Oyl/Editor"

project "TestRecompilableModule"; do
	language "C++"
	kind "SharedLib"

	oyl3d.project.files()

	-- reflection "On"

	uses {
		"Core",
		"Application",
		"Rendering",
		"EditorClient"
	}
end
