project(Refly.Core.ProjectName)
    location (Config.ProjectLocation)
    kind "SharedLib"

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
        
    }

    includedirs {
        Refly.Location,
        Refly.Core.Location,
        Refly.ThirdParty.Location,
    }

    defines {
        defineInsideMacro(Refly.Core.Name),
    }