EngineAssembly {
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
    end
}