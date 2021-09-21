project "ReflyEngine"
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
        
    }

    includedirs {
        "%{wks.location}/Source/",
        "%{wks.location}/Source/Engine/",
        "%{wks.location}/Source/ThirdParty/",
    }

    defines {
        "_INSIDE_REFLY_ENGINE=1",
    }