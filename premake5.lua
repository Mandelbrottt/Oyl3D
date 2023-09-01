include "Config.lua"
include "Packages.lua"

include "Scripts/Premake/Common.lua"
include "Scripts/Premake/Actions.lua"

if _ACTION == "clean" or _ACTION == nil then
    return
end

workspace(Rearm.Name)
    location "./"
    filename(Rearm.Name .. "_" .. _ACTION)

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

    startproject(Rearm.Name .. Rearm.Entry.Name)

    group(Rearm.Dependencies.Name)
        generateDependencies()
    group ""

    local function mainProject(projectConfig, properties)
        local cwd = os.getcwd()
        os.chdir(projectConfig.ProjectDir)
        project(projectConfig.ProjectName)
            applyCommonCppSettings(projectConfig)
            properties()
            filter {}
        os.chdir(cwd)
    end

    group(Rearm.Name)
        mainProject(Rearm.Core, function()
            kind "SharedLib"
            pchheader "pch.h"
            pchsource "pch.cpp"
            includedirs { }
            libdirs { }
            links { }
        end)
        
        mainProject(Rearm.Rendering, function()
            kind "SharedLib"
            pchheader "pch.h"
            pchsource "pch.cpp"

            VULKAN_SDK = os.getenv("VULKAN_SDK")
            includedirs { "%{VULKAN_SDK}/Include" }
            libdirs { "%{VULKAN_SDK}/Lib" }
            links { "vulkan-1" }
        end)

        mainProject(Rearm.Editor, function()
            kind "SharedLib"
            pchheader "pch.h"
            pchsource "pch.cpp"
            links {
                Rearm.Core.ProjectName,
                Rearm.Rendering.ProjectName,
            }
            includedirs {
                Rearm.Core.IncludeDir,
                Rearm.Rendering.IncludeDir,
            }
            removeconfigurations { "*" .. Config.Postfix }
        end)

        mainProject(Rearm.Entry, function()
            kind "WindowedApp"
            links {
                Rearm.Core.ProjectName,
                Rearm.Rendering.ProjectName,
            }
            includedirs {
                Rearm.Core.IncludeDir,
                Rearm.Rendering.IncludeDir
            }
            filterEditorOnly(function()
                links { Rearm.Editor.ProjectName, }
                includedirs { Rearm.Editor.IncludeDir }
            end)
        end)
        group ""

    project(Rearm.Premake.Name)
        kind "MakeFile"
        filename("%{prj.name}_" .. _ACTION)
        targetdir(Config.TargetDir .. Config.OutputDir)
        objdir(Config.ObjectDir .. Config.OutputDir)

        local runPremakeCommand = "%{wks.location}/Binaries/ThirdParty/premake5.exe " .. _ACTION

        buildcommands {
            runPremakeCommand,
        }
        rebuildcommands {
            runPremakeCommand,
        }

include "Scripts/Premake/Overrides.lua"