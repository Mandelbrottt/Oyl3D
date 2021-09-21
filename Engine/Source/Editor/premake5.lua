project "ReflyEditor"
    location (Dirs.Project)
    kind "SharedLib"

    ApplyCommonCppSettings()

    targetdir(Dirs.Target .. Dirs.Output)
    objdir   (Dirs.Object .. Dirs.Output)

    files {
        "./**.cpp",
        "./**.h",
        "./**.hpp",
        "./**.inl",
    }

    links {
        "ReflyEngine",
    }

    includedirs {
        "%{wks.location}/Source/",
        "%{wks.location}/Source/Editor/",
        "%{wks.location}/Source/ThirdParty/",
    }

    defines {
        "_INSIDE_REFLY_EDITOR=1",
    }

    removeconfigurations { "*" .. Configs.Postfix }