include "Config.lua"
include "Packages.lua"

include "Scripts/Premake/Common.lua"
include "Scripts/Premake/Actions.lua"

if _ACTION == "clean" or _ACTION == nil then
    return
end

processDependencies(Dependencies)

workspace(Config.Name)
    location "./"
    filename(Config.Name .. "_" .. _ACTION)

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

    startproject(Config.Name .. "Entry")

    group("Dependencies")
        -- generateDependencies()
    group ""

    group(Config.Name)
        local moduleScripts = os.matchfiles(path.join(Config.SourceDir, "**premake5.lua"))
        for _, moduleScript in pairs(moduleScripts) do
            include(moduleScript)
        end

        -- mainProject("RearmCore", function()
        --     kind "SharedLib"
        --     pchheader "pch.h"
        --     pchsource "pch.cpp"
        --     includedirs { }
        --     libdirs { }
        --     links { }
        -- end)

        -- mainProject(Rearm.Rendering, function()
        --     kind "SharedLib"
        --     pchheader "pch.h"
        --     pchsource "pch.cpp"

        --     VULKAN_SDK = os.getenv("VULKAN_SDK")
        --     includedirs { "%{VULKAN_SDK}/Include" }
        --     libdirs { "%{VULKAN_SDK}/Lib" }
        --     links { "vulkan-1" }
        -- end)

        -- mainProject(Rearm.Editor, function()
        --     kind "SharedLib"
        --     pchheader "pch.h"
        --     pchsource "pch.cpp"
        --     links {
        --         Rearm.Core.ProjectName,
        --         Rearm.Rendering.ProjectName,
        --     }
        --     includedirs {
        --         Rearm.Core.IncludeDir,
        --         Rearm.Rendering.IncludeDir,
        --     }
        --     removeconfigurations { "*" .. Config.Postfix }
        -- end)

        -- mainProject(Rearm.Entry, function()
        --     kind "WindowedApp"
        --     links {
        --         Rearm.Core.ProjectName,
        --         Rearm.Rendering.ProjectName,
        --     }
        --     includedirs {
        --         Rearm.Core.IncludeDir,
        --         Rearm.Rendering.IncludeDir
        --     }
        --     filterEditorOnly(function()
        --         links { Rearm.Editor.ProjectName, }
        --         includedirs { Rearm.Editor.IncludeDir }
        --     end)
        -- end)
    group ""

    project("Premake")
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