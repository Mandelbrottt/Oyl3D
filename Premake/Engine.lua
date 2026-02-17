require "Premake.Oyl"
require "Premake.Utils"

local Config = Oyl.Config

Oyl.Engine = {}
local Engine = Oyl.Engine
local Utils = Oyl.Utils

---@class Engine.Project
---@field Language string,
---@field Kind string,
---@field Dir? string
---@field Name? string
---@field ProjectName? string
---@field Group? string
---@field Pch? string
---@field Dependencies? table[string]
---@field Properties? function

---@type Engine.Project[]
Engine.Projects = {}

function Oyl.Engine.GenerateProjects()
    -- Recurse through the source directory and include all premake scripts
    local scripts = os.matchfiles(path.join(Config.SourceDir, "**premake5.lua"))
    for _, script in pairs(scripts) do
        include(script)
    end

    -- Iterate all projects, and generate their dependencies
    for name, proj in pairs(Engine.Projects) do
        project(proj.ProjectName)
        if (proj.Dependencies) then
            for _, dep in ipairs(proj.Dependencies) do
                if dep ~= name then
                    Engine.AddDependencyToProject(dep)
                else
                    premake.error(string.format("Assembly \"%s\" cannot depend on itself!", proj.Name))
                end
            end
        end
        project "*"
    end
end

---@param dep string
function Oyl.Engine.AddDependencyToProject(dep)
    filter {}

    local assembly = Utils.CaseInsensitiveFind(Engine.Projects, dep)
    if assembly then
        links { assembly.ProjectName }
        includedirs { path.join(Config.SourceDir, assembly.Dir) }
        return
    end

    local package = Utils.CaseInsensitiveFind(Oyl.Packages, dep)
    if package then
        links { package.Name }
        includedirs(package.IncludeDirs)

        if (package.DependantProperties) then
            package:DependantProperties()
            filter {}
        end
        return
    end

    local library = Utils.CaseInsensitiveFind(Oyl.Libraries, dep)
    if library then
        includedirs(library.IncludeDirs)
        libdirs(library.LibraryDirs)
        links(library.Libraries)
        return
    end
end

---@param proj Engine.Project
function Oyl.Engine.EngineProjectDefinition(proj)
    local projectDir = Utils.ScriptDir(3)
    local projectName = path.getname(projectDir)

    proj.Dir = projectDir
    proj.Name = projectName
    proj.ProjectName = string.format("%s.%s", Config.ShortName, projectName)
    proj.Group = proj.Group or ""
    proj.Kind = proj.Kind or premake.UTILITY
    proj.Dependencies = proj.Dependencies or {}

    if proj.Group then
        group(proj.Group)
    end

    project(proj.ProjectName)
        if proj.Language == premake.CPP then
            applyCommonCppSettings(proj)
        else
            premake.error(string.format("Invalid language \"%s\" in project \"%s\"", proj.Language, projectName))
        end

        kind(proj.Kind)
        if proj.Kind == premake.SHAREDLIB then
            filterStandalone()
                kind(premake.STATICLIB)
        end

        filter {}

        -- Generated Files
        includedirs { "%{prj.location}/Generated/" }
        removefiles { "**.generated.h" }

        files { "**.lua" }
        removefiles { "**/Platform/**" }

        filter "system:windows"
            files { "**/*_Windows*" }

        filter {}
        if proj.Properties then
            proj.Properties()
        end
        filter {}
    group ""

    Engine.Projects[proj.Name] = proj
end
