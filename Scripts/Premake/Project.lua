function filterEditor()
    filter(string.format("platforms:*%s*", Config.Platforms.Editor))
end

function filterStandalone()
    filter(string.format("platforms:not *%s*", Config.Platforms.Editor))
end

function defineInsideMacro(projectName)
    projectName = projectName:gsub("%.", "_")
    projectName = projectName:gsub("%-", "_")
    return string.upper(
        string.format("_INSIDE_%s_%s=1", Config.ShortName, projectName)
    )
end

function applyCommonCppSettings(assemblyDefinition)
    filename("%{prj.name}_" .. _ACTION)
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    floatingpoint "fast"
    rtti "on"
    stringpooling "on"
    
    location (Config.ProjectLocation)
    targetdir(Config.TargetDir .. Config.OutputDir)
    debugdir (Config.TargetDir .. Config.OutputDir)
    objdir   (Config.ObjectDir .. Config.OutputDir)

    files {
        "./**.cpp",
        "./**.h",
        "./**.hpp",
        "./**.inl",
        "./**.ixx",
    }

    links {
        "Premake"
    }

    if assemblyDefinition then
        includedirs {
            "%{prj.location}",
        }

        -- header files can be included across assembly boundaries, and so have to use project-agnostic includes
        -- FIXME: premake doesn't support per-file includedirs
        filter "files:**.cpp"
            includedirs {
                path.join("%{prj.location}", assemblyDefinition.Name),
            }
        filter {}

        defines {
            string.upper(Config.ShortName) .. "_CURRENT_ASSEMBLY=\"" .. assemblyDefinition.Name .. "\"",
            defineInsideMacro(assemblyDefinition.Name),

            -- Warning Silence Defines
            "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING", -- Silence warning in spdlog
            "TRACY_ENABLE", -- TODO: Move to config filter once new config setup is implemented
            "TRACY_DELAYED_INIT",
            "TRACY_MANUAL_LIFETIME",
        }
    end

    flags {
        "FatalWarnings",
        "MultiProcessorCompile",
    }

    filter "kind:StaticLib"
        targetdir(Config.LibraryDir .. Config.OutputDir)

    filter "toolset:msc*"
        disablewarnings {
            "4251", -- member needs dll-interface to be used by clients of class
            "4275", -- non dll-interface used as base for dll-interface
        }

    filter "system:windows"
        architecture "x64"

    filter("configurations:" .. Config.Configurations.Debug)
        optimize "off"
        runtime "debug"
        symbols "on"
        defines { string.upper(Config.ShortName) .. "_DEBUG=1" }

    filter("configurations:" .. Config.Configurations.Development)
        optimize "on"
        runtime "release"
        symbols "on"
        defines { string.upper(Config.ShortName) .. "_DEVELOPMENT=1" }

    filter("configurations:" .. Config.Configurations.Profile)
        optimize "on"
        runtime "release"
        symbols "on"
        defines { string.upper(Config.ShortName) .. "_PROFILE=1" }
        
    filter("configurations:" .. Config.Configurations.Distribution)
        optimize "full"
        runtime "release"
        symbols "off"
        defines { string.upper(Config.ShortName) .. "_DISTRIBUTION=1" }
        
    filterEditor()
        removeconfigurations {
            Config.Configurations.Distribution
        }
        defines { string.upper(Config.ShortName) .. "_EDITOR=1" }
        
    filterStandalone()
        staticruntime "on"

    filter {}
end

local function case_insensitive_find(table, index)
    for key, value in pairs(table) do
        if key:lower() == index:lower() then
            return value
        end
    end
end

function addDependencyToProject(dependencyName)
    local assembly = case_insensitive_find(Assemblies, dependencyName)
    if assembly then
        links { assembly.ProjectName }
        includedirs { path.join(Config.SourceDir, assembly.Dir) }
        return
    end

    local dependency = case_insensitive_find(Dependencies, dependencyName)
    if dependency then
        links { dependency.Name }
        includedirs(dependency.IncludeDirs)
        return
    end

    local library = case_insensitive_find(Libraries, dependencyName)
    if library then
        includedirs(library.IncludeDirs)
        libdirs(library.LibraryDirs)
        links(library.Libraries)
        return
    end
end