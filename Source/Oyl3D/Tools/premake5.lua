local Engine = require "Engine"

group "Executables"

startproject "Oyl.Spyll"

project "Oyl.Spyll"; do
	language "C++"
	kind "ConsoleApp"

	links {
		"Spyll",
	}
end
