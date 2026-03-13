local Config = require "Config"
local Engine = require "Engine"

-- Oyl.Projects.Editor = Engine.EngineProjectDefinition {
-- 	Group = "Editor",
-- 	Language = premake.CPP,
-- 	Kind = premake.SHAREDLIB,
-- 	Dependencies = {
-- 		Oyl.Projects.Core,
-- 		Oyl.Packages.ImGui,
-- 		Oyl.Packages.SpdLog,
-- 		Oyl.Packages.TracyClient,
-- 	},
-- 	Properties = function()
-- 		pchheader "pch.h"
-- 		pchsource "pch.cpp"
-- 		removeconfigurations {
-- 			Config.Configurations.Distribution
-- 		}
-- 		removeplatforms {
-- 			Config.Platforms.Standalone
-- 		}
-- 	end
-- }

group "Editor"

project "Editor"; do
	language(premake.CPP)
	kind(premake.SHAREDLIB)
	
	pchheader "pch.h"
	pchsource "pch.cpp"

	removeconfigurations {
		Config.Configurations.Distribution
	}
	
	removeplatforms {
		Config.Platforms.Standalone
	}
	
	Engine.SetupProject()
	Engine.Dependencies {
		Oyl.Projects.Core,
		Oyl.Packages.ImGui,
		Oyl.Packages.SpdLog,
		Oyl.Packages.TracyClient,
	}
end
