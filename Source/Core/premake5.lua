require "Premake.Engine"

Oyl.Engine.EngineProjectDefinition {
    Group = "Engine",
    Language = premake.CPP,
    Kind = premake.SHAREDLIB,
    Dependencies = {
        "SpdLog",
        "GLFW",
        "Vulkan",
		"NlohmannJson",
		"YamlCpp",
        "TracyClient",
    },
    Properties = function()
        pchheader "pch.h"
        pchsource "pch.cpp"
    end
}