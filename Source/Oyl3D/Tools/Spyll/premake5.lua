local Oyl3D = require "Oyl3D"

group "Oyl/Tools"

Oyl3D.CppProject "Oyl.Spyll"; do
	language "C++"
	kind "ConsoleApp"

	-- Needed to interfaces with Spyll.Tool
	runtime "Release"
	defines {
		"_ITERATOR_DEBUG_LEVEL=0"
	}

	links {
		"Spyll.Core",
	}
end
