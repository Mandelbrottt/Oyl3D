function filterEditorOnly(filterCallback)
    filter("configurations:not *" .. Config.Postfix .. " or *" .. Config.Editor.Postfix)
        filterCallback()
    filter {}
end

function defineInsideMacro(projectName)
    return string.upper(
        string.format("_INSIDE_%s_%s=1", Rearm.Name, projectName)
    )
end

function applyCommonCppSettings(projectConfig)
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
        Rearm.Premake.Name
    }

    for _, dependency in pairs(Dependencies) do
        links { dependency.Name }
        includedirs(dependency.IncludeDirs)
    end

    if projectConfig then
        includedirs {
            projectConfig.IncludeDir,
        }

        -- header files can be included across module boundaries, and so have to use project-agnostic includes
        filter "files:**.cpp"
            includedirs {
                projectConfig.IncludeDir .. projectConfig.Name,
            }
        filter {}

        defines {
            defineInsideMacro(projectConfig.Name),
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
        defines { string.upper(Rearm.Name) .. "_DEBUG=1" }

    filter("configurations:" .. Config.Development.Prefix .. "*")
        optimize "debug"
        runtime "release"
        defines { string.upper(Rearm.Name) .. "_DEVELOPMENT=1" }

    filter("configurations:" .. Config.Release.Prefix .. "*")
        optimize "on"
        runtime "release"
        defines { string.upper(Rearm.Name) .. "_RELEASE=1" }
        
    filterEditorOnly(function()
        defines { string.upper(Rearm.Name) .. "_EDITOR=1" }
    end)

    filter {}
end