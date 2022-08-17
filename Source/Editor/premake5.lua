project(Refly.Editor.ProjectName)
    kind "SharedLib"

    applyCommonCppSettings(Refly.Editor)

    pchheader "pch.h"
    pchsource "pch.cpp"

    links {
        Refly.Core.ProjectName,
    }

    includedirs {
        Refly.Core.Location,
    }

    removeconfigurations { "*" .. Config.Postfix }