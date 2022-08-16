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
        string.upper("_INSIDE_" .. Refly.Name .. "_" .. Refly.Editor.Name .. "=1"),
    }

    removeconfigurations { "*" .. Config.Postfix }