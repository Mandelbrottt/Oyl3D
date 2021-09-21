project "Refly-Editor"
    location (EngineProjectDir)
    kind "SharedLib"
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
    }

    includedirs {
        "%{wks.location}/Source/",
        "%{wks.location}/Source/ThirdParty",
    }

    defines {
        "REFLY_EDITOR",
    }