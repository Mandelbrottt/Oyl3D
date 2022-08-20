project(Rearm.Entry.ProjectName)
    applyCommonCppSettings(Rearm.Entry)
    
    kind "WindowedApp"

    links {
        Rearm.Core.ProjectName,
    }
    
    includedirs {
        Rearm.Core.IncludeDir,
    }

    filterEditorOnly(function()
        links { Rearm.Editor.ProjectName, }
        includedirs { Rearm.Editor.IncludeDir }
    end)