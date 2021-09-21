project "Refly-Entry"
    location (EngineProjectDir)
    kind "WindowedApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    floatingpoint "fast"

    flags {
        "FatalWarnings",
        "MultiProcessorCompile",
    }
    
    targetdir(TargetDir .. OutputDir .. "/%{prj.name}/")
    objdir   (ObjectDir .. OutputDir .. "/%{prj.name}/")

    files {
        "./**.cpp",
        "./**.h",
        "./**.hpp",
        "./**.inl",
    }

    links {
        "Refly",
        "Refly-Editor",
    }

    includedirs {
        "%{wks.location}/Source/",
        "%{wks.location}/Source/ThirdParty",
    }

    defines {
        "REFLY_ENTRY",
    }