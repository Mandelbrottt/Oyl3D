project("DllTest")
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
        Refly.Location .. "DllTest",
        Refly.ThirdParty.Location,
    }

    defines {
        defineInsideMacro("DllTest"),
    }