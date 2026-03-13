local Engine = require "Engine"
local Packages = require "Dependencies"

-- Oyl.Projects.Entry = Engine.EngineProjectDefinition {
-- 	Group = "Executables",
-- 	Language = premake.CPP,
-- 	Kind = premake.WINDOWEDAPP,
-- 	Dependencies = {
-- 		Oyl.Projects.Core,
-- 		Oyl.Projects.Editor,
-- 		Oyl.Packages.SpdLog,
-- 		Oyl.Packages.TracyClient,
-- 	},
-- 	Properties = function()
-- 		targetname(Engine.Name .. "-%{cfg.platform}")
-- 	end
-- }

group "Executables"

startproject "Entry"

project "Entry"; do
	language(premake.CPP)
	kind(premake.WINDOWEDAPP)

	targetname(Engine.Name .. "-%{cfg.platform}")

	Engine.SetupProject()
	Engine.Dependencies {
		Oyl.Projects.Core,
		Oyl.Projects.Editor,
		Oyl.Packages.SpdLog,
		Oyl.Packages.TracyClient,
	}
end
