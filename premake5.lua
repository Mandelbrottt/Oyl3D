include "Config.lua"
include "Packages.lua"

include "Scripts/Premake/Common.lua"
include "Scripts/Premake/Actions.lua"

if _ACTION == "clean" or _ACTION == nil then
    return
end

ProcessDependencies(Dependencies)

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
    
    startproject("Rearm.Entry")

    group("Dependencies")
        GenerateDependencyProjects(Dependencies)
    group ""

    local moduleScripts = os.matchfiles(path.join(Config.SourceDir, "**premake5.lua"))
    for _, moduleScript in pairs(moduleScripts) do
        include(moduleScript)
    end

    for name, module in pairsByKeys(Modules) do
        project(module.ProjectName)

        for _, dependencyName in ipairs(module.Dependencies) do
            if dependencyName == name then
                premake.error(string.format("Module \"%s\" cannot depend on itself!", module.Name))
                goto continue
            end

            AddDependencyToProject(dependencyName)

            ::continue::
        end

        project "*"
    end

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