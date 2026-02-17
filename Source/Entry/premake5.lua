local Engine = require "Engine"

Engine.EngineProjectDefinition {
    Group = "Executables",
    Language = premake.CPP,
    Kind = premake.WINDOWEDAPP,
    Dependencies = {
        "Core",
        "Editor",
        "SpdLog",
        "TracyClient",
    },
}