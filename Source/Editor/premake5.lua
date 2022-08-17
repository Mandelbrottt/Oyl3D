project(Refly.Editor.ProjectName)
    kind "SharedLib"

    applyCommonCppSettings(Refly.Editor)

    links {
        Refly.Core.ProjectName,
    }

    includedirs {
        Refly.Core.Location,
    }

    removeconfigurations { "*" .. Config.Postfix }