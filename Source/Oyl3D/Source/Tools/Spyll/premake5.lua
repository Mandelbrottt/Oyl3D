local Project = require "Project"

group "Oyl/Tools"

project "Oyl.Spyll"; do
	language "C++"
	kind "ConsoleApp"

	Project.Files()

	-- Needed to interfaces with Spyll.Tool
	runtime "Release"
	defines {
		"_ITERATOR_DEBUG_LEVEL=0"
	}

	links {
		"Spyll.Core",
	}
end
