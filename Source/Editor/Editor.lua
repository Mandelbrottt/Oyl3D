project(Refly.Editor.ProjectName)
    applyCommonCppSettings(Refly.Editor)

    kind "SharedLib"

    pchheader "pch.h"
    pchsource "pch.cpp"

    links {
        Refly.Core.ProjectName,
    }

    includedirs {
        Refly.Core.IncludeDir,
    }

    removeconfigurations { "*" .. Config.Postfix }