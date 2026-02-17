local Config = require "Config"
local Engine = require "Engine"

Engine.EngineProjectDefinition {
    Group = "Editor",
    Language = premake.CPP,
    Kind = premake.SHAREDLIB,
    Dependencies = {
        "Core",
        "ImGui",
        "SpdLog",
        "YamlCpp",
        "TracyClient",
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