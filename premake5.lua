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
    
    startproject(Config.ShortName .. ".Entry")

    group("Dependencies")
        generateDependencyProjects(Dependencies)
    group ""

    local assemblyScripts = os.matchfiles(path.join(Config.SourceDir, "**premake5.lua"))
    for _, assemblyScript in pairs(assemblyScripts) do
        include(assemblyScript)
    end

    for name, assembly in pairsByKeys(Assemblies) do
        project(assembly.ProjectName)

        for _, dependencyName in ipairs(assembly.Dependencies) do
            if dependencyName == name then
                premake.error(string.format("Assembly \"%s\" cannot depend on itself!", assembly.Name))
                goto continue
            end

            addDependencyToProject(dependencyName)

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