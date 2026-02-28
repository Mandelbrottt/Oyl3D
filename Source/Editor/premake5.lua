local Config = require "Config"
local Engine = require "Engine"

Engine.EngineProjectDefinition {
    Group = "Editor",
    Language = premake.CPP,
    Kind = premake.SHAREDLIB,
    Dependencies = {
        "Core",
        Oyl.Packages.ImGui,
        Oyl.Packages.SpdLog,
        Oyl.Packages.TracyClient,
    },
    Properties = function()
        pchheader "pch.h"
        pchsource "pch.cpp"
        removeconfigurations {
            Config.Configurations.Distribution
        }
        removeplatforms {
            Config.Platforms.Standalone
        }
    end
}