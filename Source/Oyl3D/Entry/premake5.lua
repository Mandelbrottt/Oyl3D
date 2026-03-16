local Engine = require "Engine"

group "Executables"

startproject "Oyl.Entry"

project "Oyl.Entry"; do
	language(premake.CPP)
	kind(premake.WINDOWEDAPP)

	targetname(Engine.Name .. "%{cfg.platform}")

	links {
		"Oyl.Core",
		"Oyl.Editor",
		"SpdLog",
		"TracyClient",
	}
end
