project(Refly.Entry.ProjectName)
    applyCommonCppSettings(Refly.Entry)
    
    kind "WindowedApp"

    links {
        Refly.Core.ProjectName,
    }
    
    includedirs {
        Refly.Core.IncludeDir,
    }

    filterEditorOnly(function()
        links { Refly.Editor.ProjectName, }
        includedirs { Refly.Editor.IncludeDir }
    end)