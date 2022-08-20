project(Rearm.Editor.ProjectName)
    applyCommonCppSettings(Rearm.Editor)

    kind "SharedLib"

    pchheader "pch.h"
    pchsource "pch.cpp"

    links {
        Rearm.Core.ProjectName,
    }

    includedirs {
        Rearm.Core.IncludeDir,
    }

    removeconfigurations { "*" .. Config.Postfix }