local Engine = require "Engine"

Oyl.Projects.Core = Engine.EngineProjectDefinition {
    Group = "Engine",
    Language = premake.CPP,
    Kind = premake.SHAREDLIB,
    Dependencies = {
        Oyl.Packages.SpdLog,
        Oyl.Packages.TracyClient,
        Oyl.Packages.Vulkan,
    },
    Properties = function()
        pchheader "pch.h"
        pchsource "pch.cpp"
    end
}