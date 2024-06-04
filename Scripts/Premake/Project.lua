function filterEditorOnly(filterCallback)
    filter("configurations:not *" .. Config.Postfix .. " or *" .. Config.Editor.Postfix)
        filterCallback()
    filter {}
end

function defineInsideMacro(projectName)
    projectName = projectName:gsub("%.", "_")
    projectName = projectName:gsub("%-", "_")
    return string.upper(
        string.format("_INSIDE_%s_%s=1", Config.Name, projectName)
    )
end

function applyCommonCppSettings(moduleDefinition)
    filename("%{prj.name}_" .. _ACTION)
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    floatingpoint "fast"
    
    location (Config.ProjectLocation)
    targetdir(Config.TargetDir .. Config.OutputDir)
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

    if moduleDefinition then
        includedirs {
            "%{prj.location}",
        }

        -- header files can be included across module boundaries, and so have to use project-agnostic includes
        -- FIXME: premake doesn't support per-file includedirs
        filter "files:**.cpp"
            includedirs {
                path.join("%{prj.location}", moduleDefinition.Name),
            }
        filter {}

        defines {
            Config.ShortName .. "_CURRENT_MODULE=\"" .. moduleDefinition.Name .. "\"",
            defineInsideMacro(moduleDefinition.Name),
        }
    end

    flags {
        "FatalWarnings",
        "MultiProcessorCompile",
    }

    filter "toolset:msc*"
        disablewarnings {
            "4251", -- member needs dll-interface to be used by clients of class
            "4275", -- non dll-interface used as base for dll-interface
        }

    filter("configurations:" .. Config.Debug.Prefix .. "*")
        optimize "off"
        runtime "debug"
        defines { string.upper(Config.ShortName) .. "_DEBUG=1" }

    filter("configurations:" .. Config.Development.Prefix .. "*")
        optimize "debug"
        runtime "release"
        defines { string.upper(Config.ShortName) .. "_DEVELOPMENT=1" }

    filter("configurations:" .. Config.Release.Prefix .. "*")
        optimize "on"
        runtime "release"
        defines { string.upper(Config.ShortName) .. "_RELEASE=1" }
        
    filterEditorOnly(function()
        defines { string.upper(Config.ShortName) .. "_EDITOR=1" }
    end)

    filter {}
end

local function case_insensitive_find(table, index)
    for key, value in pairs(table) do
        if key:lower() == index:lower() then
            return value
        end
    end
end

function AddDependencyToProject(dependencyName)
    local module = case_insensitive_find(Modules, dependencyName)
    if module then
        links { module.ProjectName }
        includedirs { path.join(Config.SourceDir, module.Dir) }
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