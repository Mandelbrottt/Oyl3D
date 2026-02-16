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

processPackages(Packages)

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

    group("Packages")
        generatePackageProjects(Packages)
    group ""

    local premakeScripts = os.matchfiles(path.join(Config.SourceDir, "**premake5.lua"))
    for _, premakeScript in pairs(premakeScripts) do
        include(premakeScript)
    end

    for name, assembly in pairsByKeys(Assemblies) do
        project(assembly.ProjectName)

        if (assembly.Dependencies) then
            for _, dependencyName in ipairs(assembly.Dependencies) do
                if dependencyName == name then
                    premake.error(string.format("Assembly \"%s\" cannot depend on itself!", assembly.Name))
                    goto continue
                end
                
                addDependencyToProject(dependencyName)
                
                ::continue::
            end
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