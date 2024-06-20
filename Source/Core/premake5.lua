EngineModule {
    Group = "Engine",
    Language = premake.CPP,
    Kind = premake.SHAREDLIB,
    Dependencies = {
        "SpdLog",
        "GLFW",
        "Vulkan",
		"NlohmannJson",
		"YamlCpp",
    },
    Properties = function()
        pchheader "pch.h"
        pchsource "pch.cpp"
    end
}