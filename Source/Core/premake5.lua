EngineModule {
    Group = "Engine",
    Language = premake.CPP,
    Kind = premake.SHAREDLIB,
    Dependencies = {
        "SpdLog",
        "GLFW",
        "Vulkan"
    },
    Properties = function()
        pchheader "pch.h"
        pchsource "pch.cpp"
    end
}