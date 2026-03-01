local Config = require "Config"
local Utils = require "Utils"

---@alias Engine.Project.Dependency (string | { Name: string })

---@class Engine.Project
---@field Language string,
---@field Kind string,
---@field Dir? string
---@field Name? string
---@field ProjectName? string
---@field Group? string
---@field Pch? string
---@field Dependencies? Engine.Project.Dependency[]
---@field Properties? function

Oyl.Engine = {}
local Engine = Oyl.Engine

---@type { [string]: Engine.Project }
Oyl.Projects = {}

---@type table<string, boolean>
Oyl.Engine.DependenciesSet = {}

function Engine.GenerateProjects()
    -- Recurse through the source directory and include all premake scripts
    local scripts = os.matchfiles("**premake5.lua")
    for _, script in pairs(scripts) do
        include(script)
    end

    -- Iterate all projects, and generate their dependencies
    for name, proj in pairs(Oyl.Projects) do
        project(proj.ProjectName)
        if (proj.Dependencies) then
            for _, dependency in ipairs(proj.Dependencies) do
                dependency = dependency.Name or dependency
                if dependency ~= name then
                    Engine.AddDependencyToProject(dependency)
                    Engine.DependenciesSet[dependency] = true
                else
                    premake.error(string.format("Assembly \"%s\" cannot depend on itself!", proj.Name))
                end
            end
        end
        filter {}
        project "*"
    end
end

function Engine.FilterEditor()
    filter(string.format("platforms:*%s*", Config.Platforms.Editor))
end

function Engine.FilterStandalone()
    filter(string.format("platforms:not *%s*", Config.Platforms.Editor))
end

---@param proj Engine.Project
function Engine.EngineProjectDefinition(proj)
    local projectDir = Utils.ScriptDir(3)
    local projectName = path.getname(projectDir)

    proj.Dir = projectDir
    proj.Name = projectName
    proj.ProjectName = string.format("%s.%s", Config.ShortName, projectName)
    proj.Group = proj.Group or Config.EngineDefaultGroup
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

        includedirs {
            "%{prj.location}",
            "%{prj.location}/Generated/" 
        }
        removefiles { "**.generated.h" }

        files { "**.lua" }
        removefiles { "**/Platform/**" }

        filter "system:windows"
            files { "**/*_Windows*" }

        -- header files can be included across assembly boundaries, and so have to use project-agnostic includes
        -- FIXME: premake doesn't support per-file includedirs
        filter "files:**.cpp"
            includedirs {
                path.join("%{prj.location}", proj.Dir),
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

        if proj.Kind == premake.SHAREDLIB then
            Engine.FilterStandalone()
                kind(premake.STATICLIB)
            filter ""
        end

        filter {}
        if proj.Properties then
            proj.Properties()
        end
        filter {}
    group ""

    return proj
end

---@param dependency Engine.Project.Dependency
function Engine.AddDependencyToProject(dependency)
    filter {}

    dependency = dependency.Name or dependency

    ---@type Engine.Project
    local proj = Utils.CaseInsensitiveFind(Oyl.Projects, dependency)
    if proj then
        links { proj.ProjectName }
        externalincludedirs { proj.Dir }
        return
    end
    
    ---@type Package
    local package = Utils.CaseInsensitiveFind(Oyl.Packages, dependency)
    if package and package.GenerateProject then
        links { package.Name }
        externalincludedirs(package.IncludeDirs)
        
        if (package.DependantProperties) then
            package:DependantProperties()
            filter {}
        end
        return
    end
    
    ---@type Library
    local library = Utils.CaseInsensitiveFind(Oyl.Libraries, dependency)
    if library then
        externalincludedirs(library.IncludeDirs)
        libdirs(library.LibraryDirs)
        links(library.Libraries)
        return
    end
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
    externalwarnings "Off"
    externalanglebrackets "On"

    location (Config.ProjectLocation)
    targetdir(Config.TargetDir .. Config.OutputDir)
    debugdir (Config.TargetDir .. Config.OutputDir)
    objdir   (Config.ObjectDir .. Config.OutputDir)
    implibdir(Config.LibraryDir .. Config.OutputDir)

    files {
        "./**.cpp",
        "./**.h",
        "./**.hpp",
        "./**.inl",
        "./**.ixx",
    }

    if (not _OPTIONS["no-premake-check"]) then
        links {
            "Premake"
        }
    end

    filter "toolset:clang"
    do
        if (Oyl.Packages.Llvm) then
            llvmdir(Oyl.Packages.Llvm.ProjectDir)
            llvmversion "19.1.7"
        end
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