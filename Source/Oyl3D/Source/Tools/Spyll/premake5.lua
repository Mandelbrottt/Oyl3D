local oyl3d = premake.modules.oyl3d

group "Oyl/Tools"

project "Oyl.Spyll"; do
	language "C++"
	kind "ConsoleApp"

	oyl3d.project.files()

	-- Needed to interfaces with Clang libs
	runtime "Release"
	defines {
		"_ITERATOR_DEBUG_LEVEL=0"
	}

	links {
		"Clang",
	}
end
