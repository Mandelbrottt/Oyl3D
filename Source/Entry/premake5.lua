project(Refly.Entry.ProjectName)
    location (Config.ProjectLocation)
    kind "WindowedApp"

    applyCommonCppSettings()

    targetdir(Config.TargetDir .. Config.OutputDir)
    objdir   (Config.ObjectDir .. Config.OutputDir)

    files {
        "./**.cpp",
        "./**.h",
        "./**.hpp",
        "./**.inl",
    }

    links {
        Refly.Core.ProjectName,
    }

	filterEditorOnly(function()
        links { Refly.Editor.ProjectName, }
    end)
    
    includedirs {
        Refly.Location,
        Refly.Entry.Location,
        Refly.ThirdParty.Location,
    }

    defines {
        defineInsideMacro(Refly.Entry.Name),
    }
