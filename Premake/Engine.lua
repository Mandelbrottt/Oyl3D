local Config = require "Config"
local Utils = require "Utils"

Oyl.Engine = {}
local Engine = Oyl.Engine

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

function Engine.GenerateProjects()
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
function Engine.AddDependencyToProject(dep)
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
function Engine.EngineProjectDefinition(proj)
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
            Engine.ApplyCommonCppSettings()
        else
            premake.error(string.format("Invalid language \"%s\" in project \"%s\"", proj.Language, projectName))
        end

        kind(proj.Kind)
        if proj.Kind == premake.SHAREDLIB then
            Engine.FilterStandalone()
                kind(premake.STATICLIB)
        end

        -- Generated Files
        includedirs { "%{prj.location}", "%{prj.location}/Generated/" }
        removefiles { "**.generated.h" }

        files { "**.lua" }
        removefiles { "**/Platform/**" }

        filter "system:windows"
            files { "**/*_Windows*" }

        -- header files can be included across assembly boundaries, and so have to use project-agnostic includes
        -- FIXME: premake doesn't support per-file includedirs
        filter "files:**.cpp"
            includedirs {
                path.join("%{prj.location}", proj.Name),
            }
        filter {}

        defines {
            string.upper(Config.ShortName) .. "_CURRENT_ASSEMBLY=\"" .. proj.Name .. "\"",
            Engine.DefineInsideMacro(proj.Name),

            -- Warning Silence Defines
            "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING", -- Silence warning in spdlog
        }

        filter("configurations:" .. Config.Configurations.Debug)
            defines { string.upper(Config.ShortName) .. "_DEBUG=1" }

        filter("configurations:" .. Config.Configurations.Development)
            defines { string.upper(Config.ShortName) .. "_DEVELOPMENT=1" }

        filter("configurations:" .. Config.Configurations.Profile)
            defines { string.upper(Config.ShortName) .. "_PROFILE=1", }

        filter("configurations:" .. Config.Configurations.Distribution)
            defines { string.upper(Config.ShortName) .. "_DISTRIBUTION=1" }

        Engine.FilterEditor()
            defines { string.upper(Config.ShortName) .. "_EDITOR=1" }

        filter {}
        if proj.Properties then
            proj.Properties()
        end
        filter {}
    group ""

    Engine.Projects[proj.Name] = proj
end

function Engine.FilterEditor()
    filter(string.format("platforms:*%s*", Config.Platforms.Editor))
end

function Engine.FilterStandalone()
    filter(string.format("platforms:not *%s*", Config.Platforms.Editor))
end

---@param projectName string
---@return string
function Engine.DefineInsideMacro(projectName)
    projectName = projectName:gsub("%.", "_")
    projectName = projectName:gsub("%-", "_")
    return string.upper(
        string.format("_INSIDE_%s_%s=1", Config.ShortName, projectName)
    )
end

function Engine.ApplyCommonCppSettings()
    filename("%{prj.name}_" .. _ACTION)
    language "C++"
    cppdialect "C++17"
    staticruntime "Off"
    floatingpoint "Fast"
    rtti "On"
    stringpooling "On"
    warnings "Extra"
    fatalwarnings { "All" }
    multiprocessorcompile "On"

    location (Config.ProjectLocation)
    targetdir(Config.TargetDir .. Config.OutputDir)
    debugdir (Config.TargetDir .. Config.OutputDir)
    objdir   (Config.ObjectDir .. Config.OutputDir)
    implibdir(Config.LibraryDir .. Config.OutputDir)

    files {
        "%{prj.location}/**.cpp",
        "%{prj.location}/**.h",
        "%{prj.location}/**.hpp",
        "%{prj.location}/**.inl",
        "%{prj.location}/**.ixx",
    }

    if (not _OPTIONS["no-premake-check"]) then
        links {
            "Premake"
        }
    end

    filter "kind:StaticLib"
        targetdir(Config.LibraryDir .. Config.OutputDir)

    filter "toolset:msc*"
        disablewarnings {
            "4251", -- member needs dll-interface to be used by clients of class
            "4275", -- non dll-interface used as base for dll-interface
        }

    filter "toolset:clang"
        floatingpoint "Default"

    filter "system:windows"
        architecture "x86_64"

    filter("configurations:" .. Config.Configurations.Debug)
        optimize "Off"
        runtime "Debug"
        symbols "On"

    filter("configurations:" .. Config.Configurations.Development)
        optimize "On"
        runtime "Release"
        symbols "On"

    filter("configurations:" .. Config.Configurations.Profile)
        optimize "On"
        runtime "Release"
        symbols "On"
        
    filter("configurations:" .. Config.Configurations.Distribution)
        optimize "Full"
        runtime "Release"
        symbols "Off"

    filter {}
end

return Engine