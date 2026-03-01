local Engine = require "Engine"

Oyl.Projects.Core = Engine.EngineProjectDefinition {
    Group = "Engine",
    Language = premake.CPP,
    Kind = premake.SHAREDLIB,
    Dependencies = {
        Oyl.Packages.SpdLog,
        Oyl.Packages.TracyClient,
        Oyl.Libraries.Vulkan,
    },
    Properties = function()
        pchheader "pch.h"
        pchsource "pch.cpp"
    end
}