local Engine = require "Engine"

Engine.EngineProjectDefinition {
    Group = "Executables",
    Language = premake.CPP,
    Kind = premake.WINDOWEDAPP,
    Dependencies = {
        "Core",
        "Editor",
        Oyl.Packages.SpdLog,
        Oyl.Packages.TracyClient,
    },
}