project(Rearm.Core.ProjectName)
    applyCommonCppSettings(Rearm.Core)

    kind "SharedLib"

    links {
        Rearm.Exports.ProjectName
    }

    includedirs {

    }