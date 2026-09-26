local oyl3d = premake.modules.oyl3d

group "Oyl/Executables"

startproject "Oyl.Application"

project "Oyl.Application"; do
	language "C++"
	kind "WindowedApp"

	targetname("Oyl3D%{cfg.platform}")

	oyl3d.project.files()

	-- reflection "On"

	links {
		"Oyl.Core",
		"Oyl.Rendering",
		"Oyl.Editor",
	}

	links {
		"SpdLog",
		"TracyClient",
		"Glfw",
	}
end
