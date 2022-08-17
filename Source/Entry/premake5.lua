project(Refly.Entry.ProjectName)
    kind "WindowedApp"

    applyCommonCppSettings(Refly.Entry)

    links {
        Refly.Core.ProjectName,
    }
    
    includedirs {
        Refly.Core.Location,
    }

    filterEditorOnly(function()
        links { Refly.Editor.ProjectName, }
        includedirs { Refly.Editor.Location }
    end)