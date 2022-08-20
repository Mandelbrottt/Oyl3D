project("DllTest")
    kind "SharedLib"

    applyCommonCppSettings()

    links {
        
    }

    includedirs {
        Rearm.IncludeDir .. "DllTest",
        Rearm.Core.IncludeDir,
        Rearm.ThirdParty.IncludeDir,
    }