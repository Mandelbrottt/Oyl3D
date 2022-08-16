project(Refly.Editor.ProjectName)
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
        Refly.Core.ProjectName,
    }

    includedirs {
        Refly.Location,
        Refly.Editor.Location,
        Refly.ThirdParty.Location,
    }

    defines {
        defineInsideMacro(Refly.Editor.Name),
    }

    removeconfigurations { "*" .. Config.Postfix }