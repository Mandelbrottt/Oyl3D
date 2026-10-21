local oyl3d = premake.modules.oyl3d

group "Oyl/Editor"

project "EditorClient"; do
	language "C++"
	kind "WindowedApp"

	targetname("Oyl3D")

	oyl3d.project.files()

	-- reflection "On"

	uses {
		"Core",
		"Application",
		"Rendering",
	}
end
