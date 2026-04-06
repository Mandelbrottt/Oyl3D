group "Executables"

project "Oyl.Spyll"; do
	language "C++"
	kind "ConsoleApp"

	-- Needed to interfaces with Spyll.Tool
	runtime "Release"
	defines {
		"_ITERATOR_DEBUG_LEVEL=0"
	}

	links {
		"Spyll.Core",
		"Spyll.Tool",
	}
end
