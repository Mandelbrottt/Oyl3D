local SourceDir = "%{wks.location}/Source/"

Refly = {
    Name = "Refly",
    Location = SourceDir,

    Core = {
        Name = "Core",
        Location = SourceDir .. "Core/",
    },
    Entry = {
        Name = "Entry",
        Location = SourceDir .. "Entry/",
    },
    Editor = {
        Name = "Editor",
        Location = SourceDir .. "Editor/",
    },
    ThirdParty = {
        Name = "ThirdParty",
        Location = SourceDir .. "ThirdParty",
    },
    ZeroCheck = {
        Name = "ZERO_CHECK"
    }
}

Refly.Core.ProjectName = Refly.Name .. Refly.Core.Name
Refly.Entry.ProjectName = Refly.Name .. Refly.Entry.Name
Refly.Editor.ProjectName = Refly.Name .. Refly.Editor.Name

function filterEditorOnly(filterCallback)
    filter("configurations:not *" .. Config.Postfix .. " or *" .. Config.Editor.Postfix)
        filterCallback()
    filter {}
end

function applyCommonCppSettings(projectConfig)
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
    }

    links {
        Refly.ZeroCheck.Name
    }

    includedirs {
        Refly.ThirdParty.Location,
    }

    if projectConfig then
        includedirs {
            path.join(projectConfig.Name, projectConfig.Name)
        }
    
        defines {
            defineInsideMacro(projectConfig.Name),
        }
    end

    flags {
        "FatalWarnings",
        "MultiProcessorCompile",
    }


    filter("configurations:" .. Config.Debug.Prefix .. "*")
        optimize "off"
        runtime "debug"
        defines { string.upper(Refly.Name) .. "_DEBUG=1" }

    filter("configurations:" .. Config.Development.Prefix .. "*")
        optimize "debug"
        runtime "release"
        defines { string.upper(Refly.Name) .. "_DEVELOPMENT=1" }

    filter("configurations:" .. Config.Release.Prefix .. "*")
        optimize "on"
        runtime "release"
        defines { string.upper(Refly.Name) .. "_RELEASE=1" }
        
    filterEditorOnly(function()
        defines { string.upper(Refly.Name) .. "_EDITOR=1" }
    end)
end

function defineInsideMacro(projectName)
    return string.upper(
        string.format("_INSIDE_%s_%s=1", Refly.Name, projectName)
    )
end