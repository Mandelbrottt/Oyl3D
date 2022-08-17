project("DllTest")
    kind "SharedLib"

    applyCommonCppSettings()

    links {
        
    }

    includedirs {
        Refly.Location .. "DllTest",
        Refly.Core.Location,
        Refly.ThirdParty.Location,
    }