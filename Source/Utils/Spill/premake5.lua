local Config = require "Config"

group "Utils"

project "Oyl.Spill"
    kind "ConsoleApp"

    files { "**.cs" }

    filename("%{prj.name}_" .. _ACTION)
    language "C#"
    csversion "default"
    dotnetframework "4.8.1"
    warnings "Extra"
    fatalwarnings { "All" }
    multiprocessorcompile "On"

    location (Config.ProjectLocation)
    targetdir(Config.Utils.TargetDir .. Config.Utils.OutputDir)
    debugdir (Config.Utils.TargetDir .. Config.Utils.OutputDir)
    objdir   (Config.ObjectDir .. Config.Utils.OutputDir)
    implibdir(Config.LibraryDir .. Config.Utils.OutputDir)

    links {
        "Premake"
    }

    configmap {
        [Config.Configurations.Profile] = Config.Configurations.Distribution,
    }

    filter "system:windows"
        architecture "x86_64"

    filter("configurations:" .. Config.Configurations.Debug)
        optimize "Off"
        runtime "Debug"
        symbols "On"
        defines { string.upper(Config.ShortName) .. "_DEBUG" }

    filter("configurations:" .. Config.Configurations.Development)
        optimize "Debug"
        runtime "Release"
        symbols "On"
        defines { string.upper(Config.ShortName) .. "_DEVELOPMENT" }

    filter("configurations:" .. Config.Configurations.Distribution)
        optimize "Full"
        runtime "Release"
        symbols "Off"
        defines { string.upper(Config.ShortName) .. "_DISTRIBUTION" }

    filter {}
group ""