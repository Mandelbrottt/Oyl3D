local SourceDir  = "Source/"
local BaseIncludeDir = "%{wks.location}/" .. SourceDir

Rearm = {
    Name = "Rearm",
    IncludeDir = BaseIncludeDir,
    SourceDir = SourceDir,

    Core = {
        Name = "Core",
        IncludeDir = BaseIncludeDir .. "Core/",
        ProjectDir = SourceDir .. "Core/"
    },
    Entry = {
        Name = "Entry",
        IncludeDir = BaseIncludeDir .. "Entry/",
        ProjectDir = SourceDir .. "Entry/"
    },
    Editor = {
        Name = "Editor",
        IncludeDir = BaseIncludeDir .. "Editor/",
        ProjectDir = SourceDir .. "Editor/"
    },
    Dependencies = {
        Name = "Dependencies",
        IncludeDir = BaseIncludeDir .. "Dependencies/",
        ProjectDir = SourceDir .. "Dependencies/"
    },
    ZeroCheck = {
        Name = "ZERO_CHECK"
    }
}

Rearm.Core.ProjectName = Rearm.Name .. Rearm.Core.Name
Rearm.Entry.ProjectName = Rearm.Name .. Rearm.Entry.Name
Rearm.Editor.ProjectName = Rearm.Name .. Rearm.Editor.Name

include "Project.lua"
include "Dependencies.lua"