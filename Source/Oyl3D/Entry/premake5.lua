local Engine = require "Engine"

group "Executables"

startproject "Entry"

project "Entry"; do
	language(premake.CPP)
	kind(premake.WINDOWEDAPP)

	targetname(Engine.Name .. "-%{cfg.platform}")

	links {
		"Core",
		"Editor",
		"SpdLog",
		"TracyClient",
	}
end
