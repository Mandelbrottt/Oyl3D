function filterEditorOnly(filterCallback)
    filter("configurations:not *" .. Config.Postfix .. " or *" .. Config.Editor.Postfix)
        filterCallback()
    filter {}
end

function defineInsideMacro(projectName)
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

    -- for _, dependency in pairsByKeys(Dependencies) do
    --     links { dependency.Name }
    --     includedirs(dependency.IncludeDirs)
    -- end

    if moduleDefinition then
        includedirs {
            "%{prj.location}",
        }

        -- header files can be included across module boundaries, and so have to use project-agnostic includes
        filter "files:**.cpp"
            includedirs {
                path.join("%{prj.location}", moduleDefinition.Name),
            }
        filter {}

        defines {
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
        defines { string.upper(Config.Name) .. "_DEBUG=1" }

    filter("configurations:" .. Config.Development.Prefix .. "*")
        optimize "debug"
        runtime "release"
        defines { string.upper(Config.Name) .. "_DEVELOPMENT=1" }

    filter("configurations:" .. Config.Release.Prefix .. "*")
        optimize "on"
        runtime "release"
        defines { string.upper(Config.Name) .. "_RELEASE=1" }
        
    filterEditorOnly(function()
        defines { string.upper(Config.Name) .. "_EDITOR=1" }
    end)

    filter {}
end