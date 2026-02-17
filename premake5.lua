require "Config"

require "Premake.CheckProject"
require "Premake.Engine"
require "Premake.Package"

require "Premake.Actions"
require "Premake.Common"
require "Premake.Overrides"

local Config = Oyl.Config
local Package = Oyl.Package
local Engine = Oyl.Engine
local Check = Oyl.CheckProject

workspace(Config.Name) do
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

    group "Dependencies"
        Package.GenerateProjects()
        -- Library.GenerateProjects()
        -- group("Packages")
            -- generatePackageProjects(Packages)
        -- group ""

    group "Engine"
        Engine.GenerateProjects()

    Check.Generate()


    -- for name, assembly in pairsByKeys(Assemblies) do
    --     project(assembly.ProjectName)

    --     if (assembly.Dependencies) then
    --         for _, dependencyName in ipairs(assembly.Dependencies) do
    --             if dependencyName == name then
    --                 premake.error(string.format("Assembly \"%s\" cannot depend on itself!", assembly.Name))
    --                 goto continue
    --             end

    --             addDependencyToProject(dependencyName)
                
    --             ::continue::
    --         end
    --     end

    --     project "*"
    -- end
end