local Engine = require "Engine"

Oyl.Projects.Entry = Engine.EngineProjectDefinition {
	Group = "Executables",
	Language = premake.CPP,
	Kind = premake.WINDOWEDAPP,
	Dependencies = {
		Oyl.Projects.Core,
		Oyl.Projects.Editor,
		Oyl.Packages.SpdLog,
		Oyl.Packages.TracyClient,
	},
	Properties = function()
		targetname(Engine.Name .. "-%{cfg.platform}")
	end
}