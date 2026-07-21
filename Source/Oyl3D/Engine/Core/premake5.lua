local Engine = require "Engine"

group "Oyl/Engine"

project "Oyl.Core"; do
	language "C++"
	kind "SharedLib"

	-- Engine.GenerateOylSpyllInformation()

	links {
		"SpdLog",
		"TracyClient",
		"Glfw",
	}
end
