project("DllTest")
    kind "SharedLib"

    applyCommonCppSettings()

    links {
        
    }

    includedirs {
        Refly.IncludeDir .. "DllTest",
        Refly.Core.IncludeDir,
        Refly.ThirdParty.IncludeDir,
    }