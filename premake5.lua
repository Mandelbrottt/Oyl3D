include "Config.lua"
include "Packages.lua"
include "Libraries.lua"

include "Scripts/Premake/Common.lua"
include "Scripts/Premake/Actions.lua"

if _ACTION == "clean" or _ACTION == nil then
    return
end

newoption {
    trigger     = "no-premake-check",
    description = "Disable the automatic run of premake on every compile",
}

processDependencies(Packages)

workspace(Config.Name)
    location "./"
    filename(Config.Name .. "_" .. _ACTION)

    configurations {
        Config.Configurations.Debug,
        Config.Configurations.Development,
        Config.Configurations.Profile,
        Config.Configurations.Distribution,
    }

    platforms {
        Config.Platforms.Editor,
        Config.Platforms.Standalone,
    }

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

    if not _OPTIONS["no-premake-check"] then
        project("Premake")
            kind "Makefile"
            filename("%{prj.name}_" .. _ACTION)
            targetdir(Config.TargetDir .. Config.OutputDir)
            objdir(Config.ObjectDir .. Config.OutputDir)

            local runPremakeCommand = "%{wks.location}/Binaries/ThirdParty/premake5.exe " .. table.concat(_ARGV, " ")

            buildcommands {
                runPremakeCommand,
            }
            rebuildcommands {
                runPremakeCommand,
            }

            filter "system:windows"
                architecture "x86_64"
            filter {}
    end

include "Scripts/Premake/Overrides.lua"