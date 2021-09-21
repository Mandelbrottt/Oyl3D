project "ReflyEntry"
    location (Dirs.Project)
    kind "WindowedApp"

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
    
	filterEditorOnly()
        links { "ReflyEditor", }
    filter {}
    
    includedirs {
        "%{wks.location}/Source/",
        "%{wks.location}/Source/Entry/",
        "%{wks.location}/Source/ThirdParty/",
    }

    defines {
        "_INSIDE_REFLY_ENTRY=1",
    }
