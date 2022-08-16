project(Refly.Editor.ProjectName)
    location (Config.ProjectLocation)
    kind "SharedLib"

    applyCommonCppSettings()

    links {
        Refly.Core.ProjectName,
    }

    includedirs {
        Refly.Location,
        Refly.Editor.Location,
        Refly.ThirdParty.Location,
    }

    defines {
        defineInsideMacro(Refly.Editor.Name),
    }

    removeconfigurations { "*" .. Config.Postfix }