local Engine = require "Engine"

group "Executables"

startproject "Oyl.Application"

project "Oyl.Application"; do
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
