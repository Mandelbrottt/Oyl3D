include "Config.lua"
include "Dependencies.lua"

include "Scripts/Premake/Common.lua"
include "Scripts/Premake/Actions.lua"

workspace(Refly.Name)
    location "./"

    configurations {
        Config.Debug.Editor.ConfigName,
        Config.Development.Editor.ConfigName,
        Config.Release.Editor.ConfigName,

        Config.Debug.ConfigName,
        Config.Development.ConfigName,
        Config.Release.ConfigName,
    }

    platforms {
        -- "x86",
        "x64",
    }

    filter "platforms:*32 or *86"
        architecture "x86"
    filter "platforms:*64"
        architecture "x64"
    filter {}

    startproject(Refly.Name .. Refly.Entry.Name)
    
    group(Refly.ThirdParty.Name)
        generateDependencies()
    group ""

    group(Refly.Name)
        include(Refly.Core.ProjectDir)
        include(Refly.Entry.ProjectDir)
        include(Refly.Editor.ProjectDir)
        include("Source/" .. "DllTest")
    group ""

    project(Refly.ZeroCheck.Name)
        kind "Makefile"
        targetdir(Config.TargetDir .. Config.OutputDir)
        objdir   (Config.ObjectDir .. Config.OutputDir)

        if string.startswith(_ACTION, "vs") then
            local runPremakeCommand = 
                -- "%{wks.location}/Binaries/ThirdParty/premake5.exe " .. _ACTION .. " --zero-check"
                "%{wks.location}/Binaries/ThirdParty/premake5.exe " .. _ACTION

            buildcommands {
                runPremakeCommand,
            }
            rebuildcommands {
                runPremakeCommand,
            }
        end

include "Scripts/Premake/Overrides.lua"